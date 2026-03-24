# Cuttlefish Lights V2 - Project Overview

**Current Version**: 2.0 (Matte Anthracite Update)
**Architecture**: React Web App (Frontend) <-> Web Serial API <-> Arduino (Firmware)

## 1. High-Level Architecture
This project is an audio-reactive LED controller system.
*   **Frontend**: A React-based web application (running locally or hosted) that serves as the control dashboard.
*   **Communication**: logic commands (`SET:...`) are sent over USB Serial (115200 baud) using the browser's Web Serial API.
*   **Firmware**: An Arduino Sketch running a non-blocking state machine to drive WS2812B LEDs while listening for commands.
*   **Hardware**: Arduino Uno + SparkFun Spectrum Shield (MSGEQ7) + WS2812B LED Strip.

---

## 2. Directory Structure
*   **`/ArduinoCore`**: Arduino Firmware files (`.ino`).
    *   `ArduinoCore.ino`: Main entry point. Handles Serial I/O, State Machine, and Command Parsing.
    *   `LEDBehavior.ino`: Implementation of light patterns (Rainbow, SwitchOnBeat, etc.). **Must be non-blocking**.
    *   `tools.ino`: Helper functions (`isHit`, `safeDelay`, `readSpectrum`).
*   **`/control-dashboard`**: The React Frontend.
    *   `src/App.jsx`: Main UI logic (Accordion layout).
    *   `src/services/SerialService.js`: Singleton handling Web Serial connection, handshake, and command queueing.

---

## 3. Communication Protocol (V2)
The system uses a **Unified Command Protocol** to ensure state synchronization.

### Command Format
`SET:MODE,SPEED,DIR,BRIGHTNESS,VAR1,VAR2`

*   **`MODE`**: String ID (e.g., `RAINBOW`, `SWITCH`, `OFF`).
*   **`SPEED`**: Integer (1-100).
    *   For `RAINBOW`: Raw increment (Higher = Faster).
    *   For `SNAKE`/Others: Delay based (Higher Slider = Lower Delay = Faster).
*   **`DIR`**: Integer (1 = Forward, 0 = Backward).
*   **`BRIGHTNESS`**: Integer (0-255).
*   **`VAR1`**: Generic Integer (Usage depends on mode).
    *   *Rainbow*: Density (Delta Hue).
    *   *SwitchOnBeat*: Unused (Trigger sensitivity is hardcoded or auto).
*   **`VAR2`**: Reserve generic integer.

### Handshake & Flow Control
1.  **Frontend**: Sends `?` to request attention.
2.  **Firmware**: Pauses animation, sends `ACK:PAUSED`.
3.  **Frontend**: Sends command string `<SET:...>`.
4.  **Firmware**: Parses, applies, sends `ACK`, resumes animation.
*   **Heartbeat**: Firmware sends `<RDY>` every ~200ms to verify connection.

---

## 4. Firmware Details (`ArduinoCore.ino`)
### Non-Blocking Core
Old versions used `delay()`, which made the Arduino "deaf" to serial commands during animations.
*   **`safeDelay(ms)`**: Custom function that loops `handleSerial()` while waiting.
*   **`safeShow()`**: Calls `handleSerial()` before updates.
*   **Ticks**: Patterns are broken into "Tick" functions (e.g., `rainbowTick`). `loop()` calls one tick per frame.

### Key Behaviors (`LEDBehavior.ino`)
*   **Rainbow**:
    *   Uses `VAR1` for **Density** (Zoom level of the rainbow).
    *   Direction logic inverted (Forward = `hue += inc`).
*   **SwitchOnBeat**:
    *   **Logic**: Single-Shot Latch (Schmitt Trigger).
    *   **Trigger**: Strict Hit (`Band0 > Threshold` && `MaxBand == 0`).
    *   **Debounce**: Once triggered, it **Latches** (Holds Color) for minimum 200ms. This prevents strobing/flashing during the wobbles of a bass peak. It only re-arms when the bass drops below threshold.

---

## 5. Frontend Details (`control-dashboard`)
*   **Tech**: React 18, Vite, TailwindCSS, Lucide-React (Icons).
*   **Design**: "Matte Anthracite" (Dark Slate/Zinc palette, high contrast, clean control surfaces).
*   **Components**:
    *   **Sidebar**: Global Connection status.
    *   **Accordion List**: Each mode (Rainbow, Snake, etc.) has its own expanded controls.
    *   **Conditional Inputs**: Sliders (Speed, Density) only appear if the Mode supports them (`mode.hasDensity`, etc.).

---

## 6. How to Run
1.  **Hardware**: Flash `ArduinoCore/ArduinoCore.ino` to Arduino.
2.  **Frontend**:
    ```bash
    cd control-dashboard
    npm run dev
    ```
3.  **Use**: Open localhost URL -> Click "Initialize Link" -> Select Port -> Control Lights.
