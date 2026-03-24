// Sorting.ino — Non-blocking sort visualizer
//
// sortTick() completes ONE OUTER ITERATION per call, then displays.
// This makes sorting visible in seconds rather than minutes on large strips.
//
// Algorithm selected by currentVar1:
//   0 = Bubble Sort    (one full inner pass per display)
//   1 = Selection Sort (one full scan + swap per display)
//   2 = Insertion Sort (one element insertion per display)
//   3 = Heap Sort      (one heapify/extract per display)
//
// currentDirection: true = ascending, false = descending
// When a sort finishes it re-shuffles and restarts automatically.


// ---------------------------------------------------------------------------
// Helper: Extract approximate hue directly from the CRGB array to save 300 bytes SRAM
// ---------------------------------------------------------------------------
static inline uint8_t getHue(int idx) {
  CHSV hsv = rgb2hsv_approximate(leds[idx]);
  return hsv.hue;
}

// ---------------------------------------------------------------------------
// Iterative heapify — modifies leds[] directly
// ---------------------------------------------------------------------------
void heapifyIter(int n, int root, bool maxHeap) {
  int i = root;
  while (true) {
    int best  = i;
    int left  = 2 * i + 1;
    int right = 2 * i + 2;

    if (left  < n && (maxHeap ? getHue(left)  > getHue(best) : getHue(left)  < getHue(best))) best = left;
    if (right < n && (maxHeap ? getHue(right) > getHue(best) : getHue(right) < getHue(best))) best = right;

    if (best == i) break;

    CRGB tmp  = leds[i];
    leds[i]    = leds[best];
    leds[best] = tmp;
    i = best;
  }
}


// ---------------------------------------------------------------------------
// Inline swap — swaps CRGB leds directly
// ---------------------------------------------------------------------------
static inline void sortSwap(int a, int b) {
  CRGB tmp = leds[a];
  leds[a]   = leds[b];
  leds[b]   = tmp;
}


// ---------------------------------------------------------------------------
// Main state-machine tick
// ---------------------------------------------------------------------------
void sortTick() {
  static int  lastAlgo  = -1;
  static bool done      = true;  // true on first call → shuffle
  static int  si        = 0;     // outer index (shared across algos)
  static int  heapPhase = 0;     // 0 = build heap, 1 = extract
  static int  heapK     = 0;

  int  algo = currentVar1;
  bool asc  = currentDirection;

  // ── Reset & shuffle ────────────────────────────────────────────────────
  if (algo != lastAlgo || done) {
    lastAlgo  = algo;
    done      = false;
    si        = 0;
    heapPhase = 0;
    heapK     = (algo == 3) ? (NUM_LEDS / 2 - 1) : 0;

    for (int k = 0; k < NUM_LEDS; k++) {
      leds[k] = CHSV(random(255), 255, 255);
    }
    safeShow();
    return;
  }

  // ── One outer iteration per tick ────────────────────────────────────────
  switch (algo) {

    // ── Bubble Sort ───────────────────────────────────────────────────────
    // Each tick runs one full inner pass, bubbling the largest/smallest
    // unsorted element to the end. Inner loop shrinks by 1 each pass.
    case 0:
      if (si < NUM_LEDS - 1) {
        for (int j = 0; j < NUM_LEDS - 1 - si; j++) {
          if (asc ? (getHue(j) > getHue(j + 1)) : (getHue(j) < getHue(j + 1))) {
            sortSwap(j, j + 1);
          }
        }
        si++;
      } else {
        done = true;
      }
      break;

    // ── Selection Sort ────────────────────────────────────────────────────
    // Each tick scans the unsorted region for min/max, then swaps it into
    // position si. One scan + one swap per display.
    case 1:
      if (si < NUM_LEDS - 1) {
        int target = si;
        for (int j = si + 1; j < NUM_LEDS; j++) {
          if (asc ? (getHue(j) < getHue(target)) : (getHue(j) > getHue(target))) {
            target = j;
          }
        }
        if (target != si) sortSwap(si, target);
        si++;
      } else {
        done = true;
      }
      break;

    // ── Insertion Sort ────────────────────────────────────────────────────
    // Each tick inserts element[si] into its correct position in the sorted
    // left region via adjacent swaps. Fixes original bug (wrong LED index).
    case 2:
      if (si < NUM_LEDS) {
        int j = si;
        while (j > 0 && (asc ? (getHue(j - 1) > getHue(j)) : (getHue(j - 1) < getHue(j)))) {
          sortSwap(j - 1, j);
          j--;
        }
        si++;
      } else {
        done = true;
      }
      break;

    // ── Heap Sort ─────────────────────────────────────────────────────────
    // Phase 0: one heapifyIter per tick (builds the heap downward).
    // Phase 1: one root-swap + heapifyIter per tick (extracts sorted elements).
    case 3: {
      bool maxHeap = asc;  // max-heap → ascending output
      if (heapPhase == 0) {
        if (heapK >= 0) {
          heapifyIter(NUM_LEDS, heapK, maxHeap);
          heapK--;
        } else {
          heapPhase = 1;
          heapK     = NUM_LEDS - 1;
        }
      } else {
        if (heapK > 0) {
          sortSwap(0, heapK);
          heapifyIter(heapK, 0, maxHeap);
          heapK--;
        } else {
          done = true;
        }
      }
      break;
    }
  }

  safeShow();
  safeDelay(currentSpeed);
}
