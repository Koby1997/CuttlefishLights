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
  
  // By removing highestBand() == 0, we allow bass to trigger even if cymbals/high-hats 
  // are technically louder in that exact millisecond. isHit(0) now intelligently 
  // checks against the bass band's own moving average.
  bool hitHigh = isHit(0);
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

void OGTick(bool forward, int speed) {
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
  safeDelay(speed); // Maintain user-defined pacing
}

void sevenColorsTick(bool forward, int speed) {
  int start;
  forward ? start = 0 : start = NUM_LEDS - 1;
  moveLights(forward);

  readSpectrum();
  leds[start] = lightSwitch(highestBand());
  
  safeShow();
  safeDelay(speed);
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

void paintbrushTick(bool forward, int speed) {
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

  // Response (1-10) -> 1=Floaty, 10=Snappy
  float response = constrain(currentVar2, 1, 10);

  if (currentVar1 == 1) {
    float decayResponse = constrain(currentVar3, 1, 10);
    // 1 = super floaty (0.98), 10 = fast drop (0.85) - Never drops below 85% to prevent strobe jitter
    float fadeKeep = map((int)decayResponse, 1, 10, 98, 85) / 100.0;
    // 1 = extremely gentle rise (0.10), 10 = snappy fast rise (1.0)
    float attackFade = map((int)response, 1, 10, 10, 100) / 100.0;
    static float fadeLevel[7] = {0,0,0,0,0,0,0};
    static unsigned long peakTimeFade[7] = {0,0,0,0,0,0,0};

    for (int section = 0; section < 7; section++) {
      int start = (section * sectionlength) + section;
      int end   = start + sectionlength;

      float temp = 0;
      if (sensitivity > 0) {
        temp = constrain((float)band[section] / ((float)sensitivity * 0.75), 0.0, 1.0);
      }

      // Smooth attack if higher, otherwise decay
      if (temp > fadeLevel[section]) {
        fadeLevel[section] += (temp - fadeLevel[section]) * attackFade;
        peakTimeFade[section] = millis(); // Hold peak to prevent shivering
      } else {
        // Peak hold for 80ms on micro-drops prevents the 1-frame flutter
        if (millis() - peakTimeFade[section] > 80) {
          fadeLevel[section] = fadeLevel[section] * fadeKeep;
        }
      }

      CRGB baseColor = lightSwitch(section);
      baseColor.r = baseColor.r * fadeLevel[section];
      baseColor.g = baseColor.g * fadeLevel[section];
      baseColor.b = baseColor.b * fadeLevel[section];

      for (int i = start; i < end && i < NUM_LEDS; i++) {
        leds[i] = baseColor;
      }
    }

  } else {
    // --- BOUNCE MODE ---
    static float litCount[7] = {0, 0, 0, 0, 0, 0, 0};
    float decayResponse = constrain(currentVar3, 1, 10);
    // 1 = super floaty (0.98), 10 = fast drop (0.85)
    float smoothingFactor = map((int)decayResponse, 1, 10, 98, 85) / 100.0;
    // 1 = extremely gentle rise (0.10), 10 = snappy fast rise (1.0)
    float attackFactor = map((int)response, 1, 10, 10, 100) / 100.0;

    float tLits[7] = {0,0,0,0,0,0,0};

    for (int i = 0; i < 7; i++) {
      float targetLit = 0;
      if (sensitivity > 0) {
        float temp = (float)band[i] / (float)sensitivity;
        targetLit = constrain(temp * sectionlength, 0.0, (float)sectionlength);
      }
      
      tLits[i] = targetLit;

      if (targetLit > litCount[i]) {
        litCount[i] = targetLit; // Original behavior: instant jump to peak
      } else {
        litCount[i] = litCount[i] * smoothingFactor; // Exponential Decay
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

void megaBounceTick(int dirState, int speed) {
  static float smoothedCount = 0;
  
  // 1. Calculate Amplitude based on Sensitivity/Source (VAR1)
  readSpectrum();
  float rawAmplitude = 0;
  
  if (currentVar1 == 0) {
    // Punch Mode: Bass only (Isolated to Band 0 for kicks)
    rawAmplitude = band[0];
  } else {
    // Meter Mode: Top 4 Loudest Bands Average (Fixed Ceiling)
    // Find the 4 highest amplitude bands to prevent quiet frequencies from dragging the average down.
    int top4[4] = {0, 0, 0, 0};
    for (int i = 0; i < 7; i++) {
      if (band[i] > top4[0]) {
        top4[3] = top4[2];
        top4[2] = top4[1];
        top4[1] = top4[0];
        top4[0] = band[i];
      } else if (band[i] > top4[1]) {
        top4[3] = top4[2];
        top4[2] = top4[1];
        top4[1] = band[i];
      } else if (band[i] > top4[2]) {
        top4[3] = top4[2];
        top4[2] = band[i];
      } else if (band[i] > top4[3]) {
        top4[3] = band[i];
      }
    }
    rawAmplitude = (top4[0] + top4[1] + top4[2] + top4[3]) / 4.0;
  }
  
  // 2. Map Amplitude to Target LED Count
  // dirState == 2 means Center-Out, so the max distance is half the strip size
  int maxLeds = (dirState == 2) ? (NUM_LEDS / 2) : NUM_LEDS;
  
  // Ceiling Logic
  float targetCount = 0;
  if (currentVar1 == 0) {
    // Punch Mode: Uses Sensitivity * 2 multiplier to handle aggressively mastered low-end kicks
    float bounceCeiling = (sensitivity > 0) ? (sensitivity * 2.0) : 900.0;
    targetCount = constrain((rawAmplitude / bounceCeiling) * maxLeds, 0.0, (float)maxLeds);
  } else {
    // Meter Mode: Uses a fixed hardware ceiling (1023 max, dialed to 1000 for visual punch)
    targetCount = map(constrain(rawAmplitude, 0, 1000), 0, 1000, 0, maxLeds);
  }
  
  // 3. Gravity/Smoothing (VAR3 & VAR4)
  // currentVar3 (Attack) maps to how fast it shoots up (10 = instantly 1.0)
  // currentVar4 (Decay) maps to how fast it falls (10 = instantly drops fast, 1 = floaty)
  
  // Shifted ranges down to compensate for the hardware rendering 300 LEDs at once
  // Decay: 1 = keeps 99.8% (very floaty), 10 = keeps 97.2% (old value 5)
  float decayFactor = map(constrain(currentVar4, 1, 10), 1, 10, 998, 972) / 1000.0;
  // Attack: 1 = gains 5% to target (gentle rise), 10 = gains 50% to target (still snappy)
  float attackFactor = map(constrain(currentVar3, 1, 10), 1, 10, 5, 50) / 100.0;

  // To avoid getting completely stuck, ensure a minimum drop
  if (targetCount > smoothedCount) {
    // Attack phase: Use VAR3 to dictate upward travel
    smoothedCount += (targetCount - smoothedCount) * attackFactor;
  } else {
    // Decay phase: Use VAR4 mapped factor to pull down
    smoothedCount = (smoothedCount * decayFactor);
    // Apply a minimum physical gravity drop so it eventually hits 0
    smoothedCount -= 0.5;
  }
  if (smoothedCount < 0) smoothedCount = 0;
  
  int activeCount = (int)smoothedCount;
  
  // 4. Painting the Strip
  fill_solid(leds, NUM_LEDS, CRGB::Black); // Clear canvas
  
  for (int i = 0; i < activeCount; i++) {
    // Calculate Color
    CRGB pixelColor;
    if (currentVar2 == 0) {
      // Classic Rainbow Map (Green Base -> Yellow -> Red Peak)
      // Map i relative to maxLeds
      uint8_t hue = map(i, 0, maxLeds, 96, 0); // 96 is roughly Green, 0 is Red in FastLED
      pixelColor = CHSV(hue, 255, 255);
    } else if (currentVar2 == 2) {
      // Custom Solid Color via HEX Picker (parsed via trailing Tokens 7,8,9 into currentR/G/B)
      pixelColor = CRGB(currentR, currentG, currentB);
    } else {
      // Style 3: Full-Spectrum Rainbow
      // Maps the entire 0-255 FastLED hue circle across the active bounce
      uint8_t hue = map(i, 0, maxLeds, 0, 255);
      pixelColor = CHSV(hue, 255, 255);
    }
    
    // Plot based on Direction
    if (dirState == 1) {
      // Forward
      leds[i] = pixelColor;
    } else if (dirState == 0) {
      // Backward
      int index = (NUM_LEDS - 1) - i;
      if (index >= 0 && index < NUM_LEDS) leds[index] = pixelColor;
    } else if (dirState == 2) {
      // Center-Out
      int center = NUM_LEDS / 2;
      int leftIdx = center - 1 - i;
      int rightIdx = center + i;
      if (leftIdx >= 0) leds[leftIdx] = pixelColor;
      if (rightIdx < NUM_LEDS) leds[rightIdx] = pixelColor;
    }
  }
  
  safeShow();
  safeDelay(speed);
}

void threeBounceTick() {
  readSpectrum();
  
  // Actually establish 3 identical 1/3 sections
  int sectionlength = NUM_LEDS / 3;

  float decayResponse = constrain(currentVar3, 1, 10);
  // Three Bounce segments are longer (~100 LEDs), so they need slightly higher retention math to drop at the same physical speed as Seven Bounce
  // 1 = ultra floaty (0.99), 10 = fast drop (0.88)
  float fadeKeep = map((int)decayResponse, 1, 10, 99, 88) / 100.0;
  float smoothingFactor = map((int)decayResponse, 1, 10, 99, 88) / 100.0;

  // 1 = extremely gentle rise (0.10), 10 = snappy fast rise (1.0)
  float attackFade = map(constrain(currentVar2, 1, 10), 1, 10, 10, 100) / 100.0;
  float attackFactor = map(constrain(currentVar2, 1, 10), 1, 10, 10, 100) / 100.0;
  // Explicitly boost Band 4 logic slightly before weighted sections
  float b4 = band[4] * 1.25;

  float wBand0, wBand1, wBand2;
  
  if (currentVar4 == 1) {
      // STRICT MODE: Isolate specific bands
      wBand0 = band[0]; // Pure bass
      wBand1 = band[3]; // Pure mid
      wBand2 = band[6] * 1.5; // Pure highs, boosted mathematically to trigger consistently
  } else {
      // NORMAL MODE: Original adjacent frequency bleeding math
      wBand0 = (band[0] * 0.60) + (band[1] * 0.25) + (band[2] * 0.15);
      wBand1 = (band[3] * 0.60) + (band[2] * 0.20) + (b4 * 0.20);
      wBand2 = ((band[6] * 0.60) + (band[5] * 0.25) + (b4 * 0.15)) * 1.60;
  }

  float weightedBands[3] = { wBand0, wBand1, wBand2 };

  if (currentVar1 == 1) {
    // --- FADE MODE ---
    static float fadeLevel[3] = {0,0,0};
    static unsigned long peakTimeFade[3] = {0,0,0};

    for (int section = 0; section < 3; section++) {
      // Safely distribute length up the strip across iterations
      int start = (section * sectionlength);
      int end   = start + sectionlength;
      if (section == 2) end = NUM_LEDS; // Ensure absolute final limit is hit perfectly

      float temp = 0;
      if (sensitivity > 0) {
        float mod = 1.0;
        if (section == 1) mod = 1.15;
        else if (section == 2) mod = 0.85;

        temp = constrain((float)weightedBands[section] / ((float)sensitivity * mod), 0.0, 1.0);
      }

      if (temp > fadeLevel[section]) {
        fadeLevel[section] += (temp - fadeLevel[section]) * attackFade;
        peakTimeFade[section] = millis();
      } else {
        if (millis() - peakTimeFade[section] > 80) {
          fadeLevel[section] = fadeLevel[section] * fadeKeep;
        }
      }

      CRGB baseColor;
      if (section == 0) baseColor = CRGB(currentR, currentG, currentB);
      else if (section == 1) baseColor = CRGB(currentR2, currentG2, currentB2);
      else baseColor = CRGB(currentR3, currentG3, currentB3);
      
      baseColor.r = baseColor.r * fadeLevel[section];
      baseColor.g = baseColor.g * fadeLevel[section];
      baseColor.b = baseColor.b * fadeLevel[section];

      for (int i = start; i < end && i < NUM_LEDS; i++) {
        leds[i] = baseColor;
      }
    }
  } else {
    // --- BOUNCE MODE ---
    static float litCount[3] = {0, 0, 0};

    float tLits[3] = {0,0,0};

    for (int i = 0; i < 3; i++) {
      float targetLit = 0;
      if (sensitivity > 0) {
        float mod = 1.0;
        if (i == 1) mod = 1.15;
        else if (i == 2) mod = 0.85;

        float temp = (float)weightedBands[i] / ((float)sensitivity * mod);
        targetLit = constrain(temp * sectionlength, 0.0, (float)sectionlength);
      }
      
      tLits[i] = targetLit;

      if (targetLit > litCount[i]) {
        litCount[i] = targetLit; // Original behavior: instant jump to peak
      } else {
        litCount[i] = litCount[i] * smoothingFactor;
      }

      int start  = (i * sectionlength);
      int end    = start + sectionlength - 1;
      if (i == 2) end = NUM_LEDS - 1; // Last section snaps to the finish line

      int numLit = (int)litCount[i];

      CRGB baseColor;
      if (i == 0) baseColor = CRGB(currentR, currentG, currentB);
      else if (i == 1) baseColor = CRGB(currentR2, currentG2, currentB2);
      else baseColor = CRGB(currentR3, currentG3, currentB3);

      for (int j = start; j < start + numLit && j <= end && j < NUM_LEDS; j++) {
        leds[j] = baseColor;
      }
      for (int j = start + numLit; j <= end && j < NUM_LEDS; j++) {
        leds[j] = CRGB(0, 0, 0);
      }
    }

    // White separators between sections
    for (int i = 1; i < 3; i++) {
      int idx = (((i * sectionlength) + i) - 1);
      if (idx < NUM_LEDS) leds[idx] = CRGB(100, 100, 100);
    }
  }

  safeShow();
  safeDelay(15);
}
