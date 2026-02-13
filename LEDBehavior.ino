// LEDBehavior.ino

// Helper for rainbow to maintain state
void rainbowTick(bool forward, int speed) {
  // Use FastLED's built-in rainbow generator for smoothness
  static uint8_t initialHue = 0;
  
  uint8_t inc = (speed / 10) + 1;
  
  if (!forward) { 
    initialHue += inc;
  } else {
    initialHue -= inc;
  }
  
  uint8_t deltaHue = (currentVar1 > 0) ? currentVar1 : 7;
  
  fill_rainbow(leds, NUM_LEDS, initialHue, deltaHue);
  
  safeShow();
  safeDelay(10); 
}

// ... other functions ...

void switchOnBeatTick() {
  readSpectrum();
  
  static bool isLatched = false;
  static unsigned long latchStartTime = 0;
  static int currentFade = 60; 
  
  const unsigned long MIN_LATCH_MS = 200;
  
  bool hitHigh = isHit(0) && (highestBand() == 0);
  bool hitLow = !isHit(0);
  
  if (hitHigh) {
      if (!isLatched) {
          isLatched = true;
          latchStartTime = millis();
          
          int r = random(255);
          int g = random(255);
          int b = random(255);
          fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
          
          FastLED.setBrightness(60); 
          currentFade = 60;
          safeShow();
          
      } else {
          safeDelay(10);
      }
      
  } else {
      if (isLatched && (millis() - latchStartTime > MIN_LATCH_MS)) {
          if (hitLow) {
             isLatched = false; 
          }
      }
      
      if (currentFade > 20) {
          currentFade--;
      }
      FastLED.setBrightness(currentFade);
      safeShow();
      safeDelay(5);
  }
}

void OGTick(bool forward) {
  int start;
  forward ? start = 0 : start = NUM_LEDS - 1;
  moveLights(forward);
  
  readSpectrum();
  float R = band[0] + band[1];
  float G = band[2] + band[3] + band[4];
  float B = band[5] + band[6];
  
  // Logic from old OG()
  if ((3*R)/1.25 > (G+B))
    leds[start] = CRGB(250,0,0);
  else if(G > 800)
    leds[start] = CRGB(0,250,0);
  else if(B > 300)
    leds[start] = CRGB(0,0,250);
  else
    leds[start] = CRGB(75,125,0); //yellowish

  safeShow();
  safeDelay(20); // Maintain original pacing
}

void sevenColorsTick(bool forward) {
  int start;
  forward ? start = 0 : start = NUM_LEDS - 1;
  moveLights(forward);

  readSpectrum();
  leds[start] = lightSwitch(highestBand());
  
  safeShow();
  safeDelay(20);
}

void snakeTick(bool forward, int speed) {
  // Old snake() was blocking. New one moves one pixel per tick.
  int start;
  forward ? start = 0 : start = NUM_LEDS - 1;
  moveLights(forward);
  
  readSpectrum();
  leds[start] = lightSwitch(highestBand());
  
  safeShow();
  // Map speed (1-100) to delay (100ms - 5ms)
  int delayVal = map(speed, 1, 100, 100, 5);
  safeDelay(delayVal); 
}

void sevenBounceTick() {
  readSpectrum();
  
  int howManyToLight;
  int sectionlength = NUM_LEDS/7;
  float temp;

  for(int i = 0; i < 7; i++)
  {
    temp = ((float) band[i]/(float) sensitivity);
    howManyToLight = temp * sectionlength;

    howManyToLight < 1 ? howManyToLight = 1 : NULL; // Fixed logic
    howManyToLight > sectionlength ? howManyToLight = sectionlength : NULL;

    int start = (i*sectionlength) + i; 
    int end = ((i+1) * sectionlength) + i - 1;
    int lastToLight = start + howManyToLight;

    bounce(start, end, lastToLight, i);
  }

  // White separators
  for(int i = 1; i < 7; i++) {
    int idx = (((i*sectionlength) + i) - 1);
    if(idx < NUM_LEDS) leds[idx] = CRGB(100,100,100);
  }  

  safeShow();
  safeDelay(15);
}

