#include <FastLED.h>
#define NUM_LEDS 300 //How many LEDs are on your strip
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define BRIGHTNESS 60 // Just the default for now, going to create a setting to make it variable

CRGB leds[NUM_LEDS];
int colors[NUM_LEDS];

CRGB bassColor = new CRGB(0,0,0); //used for some behaviors that use random colors
int recentValues[50];
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
  FastLED.setBrightness(BRIGHTNESS);

  //Clear the LEDs
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();

  // Serial and input setup
  Serial.begin(115200);
  Serial.println("\nListening...");
}


void loop()
{
  //allWhite();

  //bassSpeed();

  //bassStartsNewColor(true);

  //forJosiah(true);

  //goGators();

  //OG(true);

  //rainbow(true,10);
  
  //randomTransition();

  //sevenBounce();
  //delay(25);
  
  //sevenColors(false);

  //smooth(true);
  //delay(5);

  //snake(true);

  //split(true, 5);

  //switchOnBeat();    

  sort(4);
}
