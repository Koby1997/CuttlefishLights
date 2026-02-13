#include <FastLED.h>
#include <SoftwareSerial.h>

#define NUM_LEDS 300 // DIAGNOSTIC TEST: Reduced from 300 to 10 to minimize interrupt blocking time
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define DEFAULT_BRIGHTNESS 60

CRGB leds[NUM_LEDS];
uint8_t colors[NUM_LEDS]; // Optimization: changed int (2 bytes) to uint8_t (1 byte). Saves 300 bytes.

CRGB bassColor = CRGB(0,0,0); //used for some behaviors that use random colors
int recentValues[20];
int recentCount = 0;
long sensitivity;
bool aboveSensitivity;

// AUDIO INPUT SETUP
int strobe = 4;
int reset = 5;
int audio1 = A0;
int audio2 = A1;
int lightDelay = A2;
int band[7];

// Serial Communication
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
long lastSerialCheck = 0;

// State Machine
enum BehaviorMode {
  OFF,
  ALL_WHITE,
  RAINBOW,
  OG_MODE,
  SEVEN_COLORS,
  SWITCH_ON_BEAT,
  SNAKE,
  SEVEN_BOUNCE // Added for visualization
};

BehaviorMode currentMode = RAINBOW; // Default behavior
int currentSpeed = 50;
int currentBrightness = 100;
bool currentDirection = true; // true = forward, false = backward
int currentVar1 = 0; // Generic Variable 1 (e.g., Length/Sensitivity)
int currentVar2 = 0; // Generic Variable 2
BehaviorMode lastMode = OFF; // Track mode changes for optimization

//State variables for non-blocking functions
unsigned long lastUpdate = 0;
int stepIndex = 0; // Generic step counter for patterns

//For Bluetooth (Optional/Legacy protection)
SoftwareSerial hc06(2,3);
String cmd="";

// Function Prototypes
void rainbowTick(bool forward, int speed);
// Function Prototypes
void rainbowTick(bool forward, int speed);
void OGTick(bool forward);
void sevenColorsTick(bool forward);
void snakeTick(bool forward, int speed);
void switchOnBeatTick();
void sevenBounceTick();
void moveLights(bool forward);
void readSpectrum();
// fill_solid is already handled by FastLED

void setup() 
{ 
  pinMode(audio1, INPUT);
  pinMode(audio2, INPUT);
  pinMode(lightDelay, INPUT);
  pinMode(strobe, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(reset, LOW);
  digitalWrite(strobe, HIGH);
  
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(currentBrightness);

  //Clear the LEDs
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  // Serial and input setup
  // REVERTING TO 115200
  // We are now handling flow control manually (15ms delay between chars).
  // High baud rate is better here because the byte transfer is faster (86us), fitting easier into gaps.
  Serial.begin(115200);
  inputString.reserve(50); // Optimization: Reduced from 200 to 50 bytes.
  Serial.println(F("\nCuttlefish Ready. Listening for commands..."));

  //Initialize Bluetooth Serial Port
  hc06.begin(115200);
}

void loop() {
  // 1. Handle Serial Commands
  handleSerial();

  // 2. Run Current Behavior
  // using millis() for non-blocking updates inside these functions
  switch (currentMode) {
    case OFF:
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      safeShow();
      safeDelay(50); // Refresh at 20fps to ensure state but mostly listen
      break;
    case ALL_WHITE:
      // Simple static assignment, doing it once or repeatedly is fine (repeatedly uses power but ensures state)
       // Optimization: only set if changed, but for now simple is robust
      fill_solid(leds, NUM_LEDS, CRGB(240,240,140));
      safeShow();
      safeDelay(50); // Refresh at 20fps to ensure state but mostly listen
      break;
    case RAINBOW:
       rainbowTick(currentDirection, currentSpeed);
       break;
    case OG_MODE:
       OGTick(currentDirection);
       break;
    case SEVEN_COLORS:
       sevenColorsTick(currentDirection);
       break;
    case SWITCH_ON_BEAT:
       switchOnBeatTick();
       break;
    case SNAKE:
       snakeTick(currentDirection, currentSpeed);
       break;
    case SEVEN_BOUNCE:
       sevenBounceTick();
       break;
    // Add other cases here as we refactor them
    default:
       break;
       break;
  }

  // Heartbeat: Tell the PC we are alive and listening
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 200) {
    lastHeartbeat = millis();
    Serial.println("<RDY>");
  }

  // Update State Tracking
  lastMode = currentMode;
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

// Blocking Handshake Handler
void handleSerial() {
  if (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == '?') {
      Serial.println("ACK:PAUSED");

      String cmd = Serial.readStringUntil('>');

      int startIdx = cmd.indexOf('<');
      if (startIdx != -1) {
          cmd = cmd.substring(startIdx + 1);
      }

      parseCommand(cmd);
    }
  }
}

void parseCommand(String command) {
  command.trim();
  if (!command.startsWith("SET:")) return; // Silent fail for viz purity? Or print?
  
  // Extract parameters string (e.g., "RAINBOW,50,1,100,0,0")
  String params = command.substring(command.indexOf(':') + 1);

  // 1. MODE
  int comma1 = params.indexOf(',');
  if (comma1 == -1) return;
  String modeStr = params.substring(0, comma1);

  // 2. SPEED
  int comma2 = params.indexOf(',', comma1 + 1);
  if (comma2 == -1) return;
  String speedStr = params.substring(comma1 + 1, comma2);

    // 3. DIR
    int comma3 = params.indexOf(',', comma2 + 1);
    if (comma3 == -1) return;
    String dirStr = params.substring(comma2 + 1, comma3);

    // 4. BRIGHTNESS (Moved before Vars)
    int comma4 = params.indexOf(',', comma3 + 1);
    String brightStr = "";
    String var1Str = "0";
    String var2Str = "0";

    if (comma4 == -1) {
        // No more commas, so the rest is Brightness. Vars are 0.
        brightStr = params.substring(comma3 + 1);
    } else {
        brightStr = params.substring(comma3 + 1, comma4);
        
        // 5. VAR1
        int comma5 = params.indexOf(',', comma4 + 1);
        if (comma5 == -1) {
             // Rest is VAR1. VAR2 is 0.
             var1Str = params.substring(comma4 + 1);
        } else {
             var1Str = params.substring(comma4 + 1, comma5);
             var2Str = params.substring(comma5 + 1);
        }
    }

    if (modeStr == "OFF") currentMode = OFF;
    else if (modeStr == "RAINBOW") currentMode = RAINBOW;
    else if (modeStr == "WHITE") currentMode = ALL_WHITE;
    else if (modeStr == "OG") currentMode = OG_MODE;
    else if (modeStr == "SEVENCOLORS") currentMode = SEVEN_COLORS;
    else if (modeStr == "SNAKE") currentMode = SNAKE;
    else if (modeStr == "SWITCH") currentMode = SWITCH_ON_BEAT;
    else if (modeStr == "BOUNCE") currentMode = SEVEN_BOUNCE;
    
    currentSpeed = speedStr.toInt();
    if (currentSpeed <= 0) currentSpeed = 1;
    currentDirection = (dirStr.toInt() == 1);
    
    // Vars
    currentVar1 = var1Str.toInt();
    currentVar2 = var2Str.toInt();
    
    // Apply Brightness
    if (brightStr != "") {
        int b = brightStr.toInt();
        // Clamp 0-255
        if (b < 0) b = 0;
        if (b > 255) b = 255;
        currentBrightness = b;
        FastLED.setBrightness(currentBrightness);
    }
    
    Serial.println("ACK");
}

// --- SAFE WRAPPERS ---
void safeShow() {
  handleSerial(); 
  FastLED.show();
}

void safeDelay(int ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    handleSerial();
  }
}




// Function Prototypes for LEDBehavior.ino
void rainbowTick(bool forward, int speed);
void OGTick(bool forward);

// ... existing code ...
