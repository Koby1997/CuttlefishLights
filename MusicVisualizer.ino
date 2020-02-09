#include <FastLED.h>
#define NUM_LEDS 300
#define DATA_PIN 6
#define LED_TYPE WS2812B
#define BRIGHTNESS 60

CRGB leds[NUM_LEDS];
CRGB bassColor = new CRGB(0,0,0);

// AUDIO INPUT SETUP
int strobe = 4;
int reset = 5;
int audio1 = A0;
int audio2 = A1;
int lightDelay = A2;
int band[7];
int highest[7];

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

  // SERIAL AND INPUT SETUP
  Serial.begin(115200);
  Serial.println("\nListening...");
}


void loop()
{
    readSpectrum();

    //moveLightsBTF(leds, NUM_LEDS);
    //OG(0);
    //sevenColors(NUM_LEDS - 1);
    //snake();

    switchOnBeat();
    //moveLightsFTB(leds, NUM_LEDS);
    //forJosiah(0);

    FastLED.show();
    delay(getDelay());
}
