#include <FastLED.h>
#include <SoftwareSerial.h>

#define NUM_LEDS 300 // DIAGNOSTIC TEST: Reduced from 300 to 10 to minimize interrupt blocking time
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define DEFAULT_BRIGHTNESS 60

CRGB leds[NUM_LEDS];
uint8_t colors[NUM_LEDS]; // Memory Isolation Test: Commented out to save 300 bytes

CRGB bassColor = CRGB(0,0,0); //used for some behaviors that use random colors
uint16_t emaAmplitude = 0;    // Optimization: Replaces recentValues[20] array (Saves 40 bytes)
long sensitivity;
bool aboveSensitivity;

// AUDIO INPUT SETUP
int strobe = 4;
int reset = 5;
int audio1 = A0;
int audio2 = A1;
int lightDelay = A2;
uint16_t band[7];

// Serial Communication
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
  SEVEN_BOUNCE, // Added for visualization
  BASS_NEW,
  DRIFT,
  FLOW,
  SMOOTH,
  SORT,
  BUILDER
};

BehaviorMode currentMode = RAINBOW; // Default behavior

// Notice: Rainbow handles speed inversely inside its own function tick, 
// using the raw 1-100 slider value rather than the inverted delay.
int currentSpeed = 15; // Raw UI speed slider value

int currentBrightness = 128; // 50%
bool currentDirection = false; // true = forward, false = backward
int currentVar1 = 20; // Density
int currentVar2 = 50; // Generic Variable 2
int currentVar3 = 0;  // Generic Variable 3 (Used for B in RGB)
BehaviorMode lastMode = OFF; // Track mode changes for optimization

//State variables for non-blocking functions
unsigned long lastUpdate = 0;
int stepIndex = 0; // Generic step counter for patterns

// Function Prototypes
void clearLeds();
void safeDelay(int ms);
void safeShow();

void OGTick(bool forward);
void rainbowTick(bool forward, int speed);
void sevenColorsTick(bool forward);
void switchOnBeatTick();
void snakeTick(bool forward, int speed);
void sevenBounceTick();
void bassStartsNewColorTick(bool forward, int speed);
void colorDriftTick(int speed);
void colorFlowTick(bool forward, int speed);
void smoothTick(bool forward, int speed);
void sortTick();
void builderTick(bool forward, int speed);

// FastLED Wrapper Functions
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
  Serial.println(F("\nCuttlefish Ready. Listening for commands..."));
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
      fill_solid(leds, NUM_LEDS, CRGB(currentVar1, currentVar2, currentVar3));
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
    case BASS_NEW:
       bassStartsNewColorTick(currentDirection, currentSpeed);
       break;
    case DRIFT:
       colorDriftTick(currentSpeed);
       break;
    case FLOW:
       colorFlowTick(currentDirection, currentSpeed);
       break;
    case SMOOTH:
       smoothTick(currentDirection, currentSpeed);
       break;
    case SORT:
       sortTick();
       break;
    case BUILDER:
       builderTick(currentDirection, currentSpeed);
       break;
    // Add other cases here as we refactor them
    default:
       break;
  }

  // Heartbeat: Tell the PC we are alive and listening
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 200) {
    lastHeartbeat = millis();
    Serial.println(F("<RDY>"));
  }

  // Update State Tracking
  lastMode = currentMode;
}

// Blocking Handshake Handler
void handleSerial() {
  if (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == '?') {
      Serial.println(F("ACK:PAUSED"));

      // 1. Wait until we see the start marker '<' or timeout (2 seconds)
      // Discard any leftover '?' from the JS stream interval
      unsigned long waitStart = millis();
      bool foundStart = false;
      while (millis() - waitStart < 2000) {
        if (Serial.available()) {
          char c = (char)Serial.read();
          if (c == '<') {
            foundStart = true;
            break;
          }
        }
      }

      // 2. If we found '<', read until '>'
      if (foundStart) {
        Serial.setTimeout(500); // Give enough time for the full string to arrive
        String cmd = Serial.readStringUntil('>');
        parseCommand(cmd);
      } else {
        Serial.println(F("DEBUG_ERR: Timeout waiting for <"));
      }
    }
  }
}

void parseCommand(String command) {
  command.trim();
  Serial.print(F("DEBUG_RX: '"));
  Serial.print(command);
  Serial.println(F("'"));

  if (!command.startsWith("SET:")) {
      Serial.println(F("DEBUG_ERR: No SET: prefix"));
      return; 
  }
  
  // Extract parameters string (e.g. "FLOW,55,0,60,0,50")
  String params = command.substring(4);
  
  // Array to hold up to 7 parameters
  String tokens[7];
  for (int i = 0; i < 7; i++) tokens[i] = "";
  
  int t = 0;
  int startIdx = 0;
  for (unsigned int i = 0; i < params.length(); i++) {
      if (params.charAt(i) == ',') {
          tokens[t++] = params.substring(startIdx, i);
          startIdx = i + 1;
          if (t >= 7) break;
      }
  }
  // catch the last one
  if (t < 7) {
      tokens[t] = params.substring(startIdx);
  }

  String modeStr   = tokens[0];
  String speedStr  = tokens[1];
  String dirStr    = tokens[2];
  String brightStr = tokens[3];
  String var1Str   = tokens[4];
  String var2Str   = tokens[5];
  String var3Str   = tokens[6];

  if (modeStr == "" || speedStr == "" || dirStr == "") {
      Serial.println(F("DEBUG_ERR: Missing core args"));
      return; 
  }

  Serial.print(F("DEBUG_MODE: ")); Serial.println(modeStr);

  // 1. MODE
  bool matched = true;
  if (modeStr == "OFF") currentMode = OFF;
  else if (modeStr == "RAINBOW") currentMode = RAINBOW;
  else if (modeStr == "WHITE") currentMode = ALL_WHITE;
  else if (modeStr == "OG") currentMode = OG_MODE;
  else if (modeStr == "SEVENCOLORS") currentMode = SEVEN_COLORS;
  else if (modeStr == "SNAKE") currentMode = SNAKE;
  else if (modeStr == "SWITCH") currentMode = SWITCH_ON_BEAT;
  else if (modeStr == "BOUNCE") currentMode = SEVEN_BOUNCE;
  else if (modeStr == "BASSNEW") currentMode = BASS_NEW;
  else if (modeStr == "DRIFT") currentMode = DRIFT;
  else if (modeStr == "FLOW") currentMode = FLOW;
  else if (modeStr == "SMOOTH") currentMode = SMOOTH;
  else if (modeStr == "SORT")  currentMode = SORT;
  else if (modeStr == "BUILDER") currentMode = BUILDER;
  else {
      matched = false;
      Serial.println(F("DEBUG_ERR: Mode not matched!"));
  }
  
  if (matched) Serial.println(F("DEBUG_OK: Mode changed"));
    
  // 2. SPEED
  if (speedStr.length() > 0) {
      currentSpeed = speedStr.toInt();
      if (currentSpeed <= 0) currentSpeed = 1;
  }
  
  // 3. DIR
  if (dirStr.length() > 0) {
      currentDirection = (dirStr.toInt() == 1);
  }
    
  // Vars (Optional)
  currentVar1 = (var1Str.length() > 0) ? var1Str.toInt() : 0;
  currentVar2 = (var2Str.length() > 0) ? var2Str.toInt() : 0;
  currentVar3 = (var3Str.length() > 0) ? var3Str.toInt() : 0;
    
  // 4. BRIGHTNESS (Optional)
  if (brightStr.length() > 0) {
      int b = brightStr.toInt();
      if (b < 0) b = 0;
      if (b > 255) b = 255;
      currentBrightness = b;
      FastLED.setBrightness(currentBrightness);
  }
    
  Serial.println(F("ACK"));
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
