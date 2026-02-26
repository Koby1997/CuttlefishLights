#include <FastLED.h>
#include <SoftwareSerial.h>

#define NUM_LEDS 300 // DIAGNOSTIC TEST: Reduced from 300 to 10 to minimize interrupt blocking time
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define DEFAULT_BRIGHTNESS 60

CRGB leds[NUM_LEDS];
uint8_t colors[NUM_LEDS]; // Memory Isolation Test: Commented out to save 300 bytes

CRGB bassColor = CRGB(0,0,0); //used for some behaviors that use random colors
long emaAmplitude = 0;    // Optimization: Replaces recentValues[20] array (Saves 40 bytes)
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
long lastSerialCheck = 0;

// State Machine
enum BehaviorMode {
  OFF,
  SOLID_COLOR,
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
  BUILDER,
  MEGA_BOUNCE
};

BehaviorMode currentMode = RAINBOW; // Default behavior

// Notice: Rainbow handles speed inversely inside its own function tick, 
// using the raw 1-100 slider value rather than the inverted delay.
int currentSpeed = 15; // Raw UI speed slider value

int currentBrightness = 128; // 50%
int currentDirection = 1;
int currentVar1 = 20; // Density
int currentVar2 = 50; // Generic Variable 2
int currentVar3 = 0;  // Generic Variable 3 (Used for B in RGB)
int currentR = 255;
int currentG = 255;
int currentB = 255;
BehaviorMode lastMode = OFF; // Track mode changes for optimization

//State variables for non-blocking functions
unsigned long lastUpdate = 0;
int stepIndex = 0; // Generic step counter for patterns

// Function Prototypes
void clearLeds();
void safeDelay(int ms);
void safeShow();

void OGTick(bool forward, int speed);
void rainbowTick(bool forward, int speed);
void sevenColorsTick(bool forward, int speed);
void switchOnBeatTick();
void snakeTick(bool forward, int speed);
void sevenBounceTick();
void bassStartsNewColorTick(bool forward, int speed);
void colorDriftTick(int speed);
void colorFlowTick(bool forward, int speed);
void smoothTick(bool forward, int speed);
void sortTick();
void builderTick(bool forward, int speed);
void megaBounceTick(int dirState, int speed);

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
    case SOLID_COLOR:
      // Simple static assignment, doing it once or repeatedly is fine (repeatedly uses power but ensures state)
       // Optimization: only set if changed, but for now simple is robust
      fill_solid(leds, NUM_LEDS, CRGB(currentR, currentG, currentB));
      safeShow();
      safeDelay(50); // Refresh at 20fps to ensure state but mostly listen
      break;
    case RAINBOW:
       rainbowTick(currentDirection, currentSpeed);
       break;
    case OG_MODE:
       OGTick(currentDirection, currentSpeed);
       break;
    case SEVEN_COLORS:
       sevenColorsTick(currentDirection, currentSpeed);
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
    case MEGA_BOUNCE:
       // Mega Bounce has a 3-way direction toggle (0=Backward, 1=Forward, 2=Center-Out)
       megaBounceTick(currentDirection, currentSpeed);
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

void parseCommand(String& command) {
  command.trim();
  // Serial.print(F("DEBUG_RX: '"));
  // Serial.print(command);
  // Serial.println(F("'"));

  if (!command.startsWith("SET:")) {
      // Serial.println(F("DEBUG_ERR: No SET: prefix"));
      return; 
  }
  
  int startIdx = 4; // Skip "SET:"
  int t = 0;
  
  String modeStr = "";
  
  while (startIdx < command.length() && t < 10) {
      int commaIdx = command.indexOf(',', startIdx);
      String token;
      if (commaIdx == -1) {
          token = command.substring(startIdx);
          startIdx = command.length();
      } else {
          token = command.substring(startIdx, commaIdx);
          startIdx = commaIdx + 1;
      }
      
      if (t == 0) {
          modeStr = token;
      } else if (t == 1 && token.length() > 0) {
          currentSpeed = token.toInt();
          if (currentSpeed <= 0) currentSpeed = 1;
      } else if (t == 2 && token.length() > 0) {
          currentDirection = token.toInt();
      } else if (t == 3 && token.length() > 0) {
          int b = token.toInt();
          if (b < 0) b = 0;
          if (b > 255) b = 255;
          currentBrightness = b;
          FastLED.setBrightness(currentBrightness);
      } else if (t == 4 && token.length() > 0) {
          currentVar1 = token.toInt();
      } else if (t == 5 && token.length() > 0) {
          currentVar2 = token.toInt();
      } else if (t == 6 && token.length() > 0) {
          currentVar3 = token.toInt();
      } else if (t == 7 && token.length() > 0) {
          currentR = token.toInt();
      } else if (t == 8 && token.length() > 0) {
          currentG = token.toInt();
      } else if (t == 9 && token.length() > 0) {
          currentB = token.toInt();
      }
      t++;
  }

  if (modeStr == "") {
      // Serial.println(F("DEBUG_ERR: Missing core args"));
      return; 
  }

  // Serial.print(F("DEBUG_MODE: ")); Serial.println(modeStr);

  bool matched = true;
  if (modeStr == "OFF") currentMode = OFF;
  else if (modeStr == "RAINBOW") currentMode = RAINBOW;
  else if (modeStr == "SOLID") currentMode = SOLID_COLOR;
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
  else if (modeStr == "MEGABOUNCE") currentMode = MEGA_BOUNCE;
  else {
      matched = false;
      // Serial.println(F("DEBUG_ERR: Mode not matched!"));
  }
  
  // if (matched) Serial.println(F("DEBUG_OK: Mode changed"));
    
  Serial.println(F("ACK")); // Keep ACK so React dashboard keeps sending slider changes seamlessly
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
