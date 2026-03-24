# Cuttlefish Lights

An immersive, audio-reactive LED controller system powered by an **Arduino UNO** and the **MSGEQ7 Spectrum Shield**. 
This project features a completely custom, cross-platform **React & Electron** desktop application to seamlessly control lighting modes, color palettes, and reactivity behaviors in real-time over Serial communication.

---

## ✨ Features
- **🎛️ Real-Time Audio Reactivity:** Splits aux audio into 7 distinct frequency bands via hardware integration.
- **🖥️ Desktop Command Center:** A beautiful, responsive Electron application to control the light strip natively on your computer.
- **🎨 Dynamic Visualizations:** Syncs frontend visualizers perfectly with the Arduino state.
- **🔄 Auto-Updating Pipeline:** Integrated with GitHub Actions CI/CD to automatically build and distribute Mac and Windows installers.

---

## 🛠 Hardware Requirements
* **Arduino UNO**
* **SparkFun Spectrum Shield** (Uses the MSGEQ7 chip)
* **WS2812B LED Strips** (e.g. ALITOVE 300 LEDs)
* **5V 10A Power Supply** (Highly recommended to prevent dimming/red-shifting)
* **Aux Splitter**
* **Male-to-Male & Male-to-Female Jumper Wires**
* **1000uF Capacitor** (Protects the LEDs during initial power surges)

---

## 🔌 Hardware Wiring
If you are assembling this for the first time, follow these core connections:
- **LED Data Line**: Connect the green data wire from your LED strip to **Digital Pin 6** on the Arduino.
- **Spectrum Shield**: Solder header pins onto the Shield and stack it directly on top of the Arduino UNO.
- **Aux Audio**: Connect the Aux Splitter to your audio source. Send one cord to your speakers, and the other cord into the Spectrum Shield's audio input port.
- **Power**: Connect the red and black power wires of the LED strip to your separate 5V Power Supply. 
- **CRITICAL**: You *must* run a wire connecting the Arduino's `GND` pin to the Power Supply's Ground! If they do not share the exact same ground, the lights will flicker wildly.

---

## 🚀 Installation & Usage

### 1. Flashing the Firmware
The entire brains of the LED operation live in the `ArduinoCore` folder.
1. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Open `ArduinoCore/ArduinoCore.ino` in the IDE.
3. **Select your Board**: Go to `Tools > Board > Arduino AVR Boards` and select **Arduino Uno**.
4. **Select your Port**: Plug in your Arduino via USB. Go to `Tools > Port` and tap the active COM port.
5. **Install FastLED**: Go to `Sketch > Include Library > Manage Libraries...`. Search for **FastLED** and click Install.
6. Click the circular **Upload** arrow button in the top left corner to compile and flash the code!

### 2. Launching the Desktop App
The desktop application is built with React, Vite, and tailwind.
1. Open a terminal and navigate to the UI folder:
   ```bash
   cd control-dashboard
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Boot up the application:
   ```bash
   npm run electron:dev
   ```

*(To automatically package the app into a `.exe` or `.dmg` installer, simply push a new `#.#.#` version tag to GitHub and the cloud-builder will handle it!)*

---

## 🔧 Troubleshooting
| Problem | Solution |
| :--- | :--- |
| **Lights are flashing randomly** | You likely have a bad ground connection. Ensure the Arduino AND the separate Power Supply share exactly the same ground line. |
| **Lights go crazy / turn pure white** | Your data line (`Pin 6`) is receiving interference, or you forgot to physically tie the grounds together. |
| **Lights dim and turn red down the strip** | Your LED strip is drawing more amperage than your power supply can handle. Lower the overall brightness in the dashboard or upgrade your power adapter. |
