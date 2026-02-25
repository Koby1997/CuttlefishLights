// LEDBehavior.ino

// Helper for rainbow to maintain state
void rainbowTick(bool forward, int speed) {
  static uint8_t initialHue = 0;
  
  uint8_t inc = (speed / 10) + 1;
  
  if (!forward) { 
    initialHue += inc;
  } else {
    initialHue -= inc;
  }
  
  // The UI slider sends 1-50.
  // Instead of an integer step per LED (which limits how wide the gradient can be),
  // we calculate the TOTAL hue spread across the entire strip.
  // Slider = 1: total spread = 15 hue units (super wide, almost solid color) 
  // Slider = 50: total spread = 750 hue units (~3 full rainbow wraps)
  // Slider = 100: total spread = 1500 hue units (~6 full rainbow wraps, extremely tight)
  uint16_t totalSpread = currentVar1 * 15;
  
  // Calculate fractional hue step per LED in 8.8 fixed-point format.
  // We MUST cast totalSpread to uint32_t before shifting, otherwise the 16-bit 
  // AVR microcontroller overflows the math during intermediate calculation!
  uint16_t hueStep88 = ((uint32_t)totalSpread << 8) / NUM_LEDS;
  uint16_t currentHue88 = initialHue << 8;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(currentHue88 >> 8, 255, 255);
    currentHue88 += hueStep88;
  }
  
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
  // A true "Snake" that slithers around the strip, leaving a trail
  static int head = 0;
  
  // The UI length slider (1-100) dictates the physical length of the snake!
  // Slider 1 (Left/Shorter) = 2 pixels. Slider 100 (Right/Longer) = Half strip.
  int snakeLength = map(currentVar1, 1, 100, 2, NUM_LEDS / 2);
  if (snakeLength <= 0) snakeLength = 10; // Fallback
  
  // Turn off the tail (the LED at the very end of the snake)
  int tail = forward ? (head - snakeLength) : (head + snakeLength);
  
  // Handle wraparound for the tail
  if (tail < 0) tail += NUM_LEDS;
  tail = tail % NUM_LEDS;
  
  leds[tail] = CRGB::Black;
  
  // Move the head forward or backward
  if (forward) {
    head++;
    if (head >= NUM_LEDS) head = 0;
  } else {
    head--;
    if (head < 0) head = NUM_LEDS - 1;
  }
  
  // Calculate the color for the new head position based on sub-mode (currentVar2)
  switch (currentVar2) {
    case 0: // Audio
      readSpectrum();
      leds[head] = lightSwitch(highestBand());
      break;
    case 1: // Moving Flowing Rainbow
      // The snake's body paints a perfect rainbow gradient from head to tail over time.
      {
        static uint8_t rainbowHue = 0;
        rainbowHue += (256 / snakeLength);
        leds[head] = CHSV(rainbowHue, 255, 255);
      }
      break;
    case 2: // Locked Bow (Matches exactly the length of the snake and moves with it)
      // We repaint the entire snake body from head to tail every tick.
      // The head (i=0) gets Hue 0 (Red). The tail gets Hue 255 (Red wrap-around).
      for (int i = 0; i < snakeLength; i++) {
        int idx = forward ? (head - i) : (head + i);
        if (idx < 0) idx += NUM_LEDS;
        idx %= NUM_LEDS; // Wrap around
        uint8_t hue = (i * 255) / snakeLength;
        leds[idx] = CHSV(hue, 255, 255);
      }
      break;
    case 3: // Random
      leds[head] = CHSV(random8(), 255, 255);
      break;
    default:
      leds[head] = CRGB::White;
      break;
  }
  
  safeShow();
  // 'speed' is the pre-calculated delay in ms from the frontend
  safeDelay(speed); 
}

void builderTick(bool forward, int speed) {
  // A single pointer that runs down the strip, permanently painting the current color
  static int pointer = 0;

  // Calculate the color for the current pointer position based on audio
  readSpectrum();
  leds[pointer] = lightSwitch(highestBand());

  // Move the pointer
  if (forward) {
    pointer++;
    if (pointer >= NUM_LEDS) {
      pointer = 0;
      // Typewriter Mode: If VAR1 is 1, clear the canvas before restarting
      if (currentVar1 == 1) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
      }
    }
  } else {
    pointer--;
    if (pointer < 0) {
      pointer = NUM_LEDS - 1;
      // Typewriter Mode: If VAR1 is 1, clear the canvas before restarting
      if (currentVar1 == 1) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
      }
    }
  }

  safeShow();
  safeDelay(speed);
}

void sevenBounceTick() {
  readSpectrum();
  int sectionlength = NUM_LEDS / 7;

  // Response (1-100) → decay speed. 1=Smooth (slow decay), 100=Snappy (fast decay).
  float response = constrain(currentVar2, 1, 100);

  if (currentVar1 == 1) {
    // --- FADE MODE ---
    // fadeKeep: how much brightness survives each tick (0.7=fast, 0.997=very slow)
    float fadeKeep = 1.0 - (response / 100.0) * 0.3;

    for (int section = 0; section < 7; section++) {
      int start = (section * sectionlength) + section;
      int end   = start + sectionlength;

      if (isLightHit(section)) {
        // Snap to full color instantly on a hit
        for (int i = start; i < end && i < NUM_LEDS; i++) {
          leds[i] = lightSwitch(section);
        }
      } else {
        // Decay at rate controlled by Response
        for (int i = start; i < end && i < NUM_LEDS; i++) {
          leds[i].r = leds[i].r * fadeKeep;
          leds[i].g = leds[i].g * fadeKeep;
          leds[i].b = leds[i].b * fadeKeep;
        }
      }
    }

  } else {
    // --- BOUNCE MODE ---
    // Instant attack: lit count jumps up to target immediately.
    // Controlled decay: lit count falls by `decayPerTick` LEDs each frame.
    static float litCount[7] = {0, 0, 0, 0, 0, 0, 0};

    // 1 = slow fall (smooth), 100 = fast fall (snappy)
    float decayPerTick = 0.1 + (response / 100.0) * 0.9;

    for (int i = 0; i < 7; i++) {
      float targetLit = 0;
      if (sensitivity > 0) {
        float temp = (float)band[i] / (float)sensitivity;
        targetLit = constrain(temp * sectionlength, 0.0, (float)sectionlength);
      }

      if (targetLit >= litCount[i]) {
        litCount[i] = targetLit; // Jump up instantly on a hit
      } else {
        litCount[i] = max(litCount[i] - decayPerTick, targetLit); // Decay
      }

      int start  = (i * sectionlength) + i;
      int end    = ((i + 1) * sectionlength) + i - 1;
      int numLit = (int)litCount[i];

      for (int j = start; j < start + numLit && j <= end && j < NUM_LEDS; j++) {
        leds[j] = lightSwitch(i);
      }
      for (int j = start + numLit; j <= end && j < NUM_LEDS; j++) {
        leds[j] = CRGB(0, 0, 0);
      }
    }

    // White separators between sections
    for (int i = 1; i < 7; i++) {
      int idx = (((i * sectionlength) + i) - 1);
      if (idx < NUM_LEDS) leds[idx] = CRGB(100, 100, 100);
    }
  }

  safeShow();
  safeDelay(15);
}

// SMOOTH: Colors based on audio bands 0, 3, 6 mapped to R, G, B.
// Each color channel's brightness is proportional to its band's amplitude vs sensitivity.
void smoothTick(bool forward, int speed) {
  int start;
  forward ? start = 0 : start = NUM_LEDS - 1;
  moveLights(forward);

  readSpectrum();

  float R = 0, G = 0, B = 0;
  if (sensitivity > 0) {
    R = (band[0]) * (255.0 / (float)sensitivity);
    G = (band[3]) * (255.0 / (float)sensitivity);
    B = (band[6]) * (255.0 / (float)sensitivity);
    if (R > 255) R = 255;
    if (G > 255) G = 255;
    if (B > 255) B = 255;
  }

  leds[start] = CRGB((uint8_t)R, (uint8_t)G, (uint8_t)B);

  safeShow();
  safeDelay(speed);
}

void bassStartsNewColorTick(bool forward, int speed) {
  int start;
  forward ? start = 0 : start = NUM_LEDS - 1;
  moveLights(forward);
  
  readSpectrum();
  
  static CRGB flowColor = CRGB(0,0,0);
  static bool wasHit = false;
  
  // Logic: Change color only on a "Rising Edge" of the beat 
  // (i.e., it wasn't hit last frame, but is hit now)
  if (isHit(0)) {
    if (!wasHit) {
      flowColor = CHSV(random(255), 255, 255);
      wasHit = true;
    }
  } else {
    wasHit = false;
  }
  
  // Inject the current flow color at the start of the strip
  leds[start] = flowColor;
  
  safeShow();
  
  // Frontend already inverts speed for delay-based modes (sends 105 - slider),
  // so use it directly as a ms delay value.
  safeDelay(speed);
}

// COLOR DRIFT: Whole strip slowly fades between random colors.
// Non-blocking version of original randomTransition().
// Goals are always vivid HSV colors so FastLED.setBrightness() works predictably.
void colorDriftTick(int speed) {
  // Use a helper to convert a hue to a vivid CRGB
  auto hueToGoal = [](int& gR, int& gG, int& gB) {
    CRGB c = CHSV(random(255), 255, 255);
    gR = c.r; gG = c.g; gB = c.b;
  };

  static int goalR, goalG, goalB;
  static int r = 0, g = 0, b = 0;
  static bool initialized = false;
  if (!initialized) { hueToGoal(goalR, goalG, goalB); initialized = true; }

  // Step r/g/b one unit toward goal each tick
  if (r < goalR) r++; else if (r > goalR) r--;
  if (g < goalG) g++; else if (g > goalG) g--;
  if (b < goalB) b++; else if (b > goalB) b--;

  // When we've reached the goal, pick a new vivid random target
  if (r == goalR && g == goalG && b == goalB) {
    hueToGoal(goalR, goalG, goalB);
  }

  fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  safeShow();
  safeDelay(speed);
}


// COLOR FLOW: A transitioning color flows down the strip.
// Non-blocking version of original randomTransition2().
// Goals are always vivid HSV colors so FastLED.setBrightness() works predictably.
void colorFlowTick(bool forward, int speed) {
  auto hueToGoal = [](int& gR, int& gG, int& gB) {
    CRGB c = CHSV(random(255), 255, 255);
    gR = c.r; gG = c.g; gB = c.b;
  };

  static int goalR, goalG, goalB;
  static int r = 0, g = 0, b = 0;
  static bool initialized = false;
  if (!initialized) { hueToGoal(goalR, goalG, goalB); initialized = true; }

  // Step r/g/b one unit toward goal each tick
  if (r < goalR) r++; else if (r > goalR) r--;
  if (g < goalG) g++; else if (g > goalG) g--;
  if (b < goalB) b++; else if (b > goalB) b--;

  // When we've reached the goal, pick a new random target
  if (r == goalR && g == goalG && b == goalB) {
    goalR = random(255);
    goalG = random(255);
    goalB = random(255);
  }

  // Flow: shift all LEDs and inject the current transitioning color at the entry point
  moveLights(forward);
  if (forward) {
    leds[0] = CRGB(r, g, b);
  } else {
    leds[NUM_LEDS - 1] = CRGB(r, g, b);
  }

  safeShow();
  safeDelay(speed);
}
