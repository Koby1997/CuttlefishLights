import { useState, useEffect } from 'react';
import { serialService } from './services/SerialService';
import {
  Zap, Power, Activity, Disc, AlignJustify,
  Palette, Music, Waves, ArrowRightLeft, Sun,
  LayoutDashboard, Settings, ChevronRight
} from 'lucide-react';
import { clsx } from 'clsx';

function App() {
  const [connected, setConnected] = useState(false);

  // Modes Definition
  const modes = [
    {
      id: "RAINBOW",
      title: "Rainbow Flow",
      icon: Palette,
      desc: "Smoothly cycling colors moving across the entire strip.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      hasDensity: true // Mapped to VAR1
    },
    {
      id: "OG",
      title: "React: OG",
      icon: Music,
      desc: "The original audio-reactive mode. Pulses with the beat.",
      hasSpeed: false,
      hasDirection: true,
      hasBrightness: true
    },
    {
      id: "SEVENCOLORS",
      title: "Seven Colors",
      icon: Disc,
      desc: "Cycles through 7 distinct colors with hard transitions.",
      hasSpeed: false,
      hasDirection: true,
      hasBrightness: true
    },
    {
      id: "SNAKE",
      title: "Bio-Snake",
      icon: Waves,
      desc: "A single 'snake' of light that slithers based on audio intensity.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true
    },
    {
      id: "BOUNCE",
      title: "Seven Bounce",
      icon: Activity,
      desc: "Bouncing balls of light that react to frequency bands.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: true
    },
    {
      id: "SWITCH",
      title: "Switch On Beat",
      icon: Zap,
      desc: "Flashes the entire strip with a random color when a beat is detected.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: false
    },
    {
      id: "WHITE",
      title: "Reading Light",
      icon: Sun,
      desc: "Solid white light for visibility.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: true
    },
    {
      id: "OFF",
      title: "System Off",
      icon: Power,
      desc: "Turn off all LEDs.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: false
    }
  ];

  const [activeMode, setActiveMode] = useState("OFF"); // What is running on the device
  const [selectedMode, setSelectedMode] = useState(modes[0].id); // What is being viewed on UI

  // Temporary states for the controls
  const [tempSpeed, setTempSpeed] = useState(50); // 1-100
  const [direction, setDirection] = useState(1); // 1 = Forward, 0 = Backward
  const [tempBrightness, setTempBrightness] = useState(60); // 0-255
  const [tempVar1, setTempVar1] = useState(0); // Generic Variable 1

  const handleConnect = async () => {
    try {
      await serialService.connect();
      setConnected(true);
    } catch (e) {
      alert("Could not connect: " + e.message);
    }
  };

  const handleApplyMode = async (modeId) => {
    if (!connected) return;

    const mode = modes.find(m => m.id === modeId);
    if (!mode) return;

    // Speed Logic:
    let delay;
    if (mode.id === "RAINBOW") {
      delay = tempSpeed; // Raw 1-100
    } else {
      // Invert for delay-based modes: 100 -> 5ms, 1 -> 100ms
      delay = mode.hasSpeed ? (105 - tempSpeed) : 50;
    }

    // Direction: Default to 1 if not applicable
    const dirVal = mode.hasDirection ? direction : 1;

    // Brightness: Default to 60 or tempBrightness
    const brightVal = tempBrightness;

    // VAR1 / VAR2
    const var1 = tempVar1;
    const var2 = 0;

    // Send the Single Unified Command
    await serialService.sendConfig(mode.id, delay, dirVal, brightVal, var1, var2);
    setActiveMode(mode.id);
  };

  const currentMode = modes.find(m => m.id === selectedMode) || modes[0];

  return (
    <div className="flex h-screen bg-zinc-950 text-zinc-200 font-sans overflow-hidden">

      {/* SIDEBAR */}
      <aside className="w-72 bg-zinc-900 border-r border-zinc-800 flex flex-col z-20 shadow-2xl">

        {/* Header */}
        <div className="p-6 pb-4">
          <div className="flex items-center gap-3 mb-2">
            <div className="w-8 h-8 bg-orange-500 rounded-lg flex items-center justify-center shadow-lg shadow-orange-900/20">
              <LayoutDashboard className="text-white" size={20} />
            </div>
            <h1 className="text-xl font-black tracking-tight text-white">
              Cuttlefish<span className="text-orange-400">Ctrl</span>
            </h1>
          </div>
          <p className="text-xs text-zinc-500 font-mono uppercase tracking-widest pl-11">
            v2.2 // Sidebar Nav
          </p>
        </div>

        {/* Global Controls & Connection */}
        <div className="px-6 mb-6">
          <div className="bg-zinc-800/50 p-4 rounded-xl border border-zinc-700">
            <div className="flex items-center justify-between mb-4">
              <span className="text-xs font-bold uppercase text-zinc-500 tracking-wider">Status</span>
              <div className={`w-2 h-2 rounded-full ${connected ? 'bg-orange-400 shadow-[0_0_8px_#fb923c]' : 'bg-red-400'}`} />
            </div>
            <button
              onClick={connected ? () => window.location.reload() : handleConnect}
              className={clsx(
                "w-full py-2.5 rounded-lg font-bold text-xs uppercase tracking-wide transition-all border",
                connected
                  ? "bg-zinc-700 text-zinc-300 border-zinc-600 hover:bg-zinc-600"
                  : "bg-orange-500 text-white border-transparent hover:bg-orange-400 shadow-lg shadow-orange-900/20"
              )}
            >
              {connected ? "Disconnect" : "Initialize Link"}
            </button>
          </div>
        </div>

        {/* Navigation List */}
        <div className="flex-1 overflow-y-auto px-4 pb-4 space-y-1">
          <div className="px-2 mb-2 text-xs font-bold text-zinc-500 uppercase tracking-wider">
            Library
          </div>
          {modes.map((mode) => {
            const isSelected = selectedMode === mode.id;
            const isActive = activeMode === mode.id;
            const Icon = mode.icon;

            return (
              <button
                key={mode.id}
                onClick={() => setSelectedMode(mode.id)}
                className={clsx(
                  "w-full flex items-center gap-3 px-3 py-3 rounded-lg text-sm font-medium transition-all group relative",
                  isSelected
                    ? "bg-zinc-800 text-white shadow-sm border border-zinc-700/50"
                    : "text-zinc-400 hover:bg-zinc-800/50 hover:text-zinc-200 border border-transparent"
                )}
              >
                {isActive && (
                  <div className="absolute left-0 top-1/2 -translate-y-1/2 w-1 h-8 bg-orange-500 rounded-r-md shadow-[0_0_8px_#f97316]" />
                )}

                <Icon size={18} className={clsx(
                  "transition-colors",
                  isSelected ? "text-orange-400" : "text-zinc-500 group-hover:text-zinc-400"
                )} />

                <span className="flex-1 text-left">{mode.title}</span>

                {isActive && (
                  <div className="w-1.5 h-1.5 rounded-full bg-orange-500 shadow-[0_0_5px_#f97316]" />
                )}
              </button>
            );
          })}
        </div>

        {/* Footer */}
        <div className="p-4 border-t border-zinc-800 text-center">
          <div className="text-[10px] text-zinc-600 font-mono">
            {connected ? "LINK ESTABLISHED" : "WAITING FOR CONNECTION"}
          </div>
        </div>
      </aside>

      {/* MAIN CONTENT AREA */}
      <main className="flex-1 bg-zinc-950 overflow-y-auto relative">
        {/* Background decorative elements */}
        <div className="absolute top-0 left-0 w-full h-96 bg-gradient-to-b from-zinc-900 to-transparent pointer-events-none" />

        <div className="max-w-4xl mx-auto p-12 relative z-10">

          {/* Header Section */}
          <header className="mb-12">
            <div className="inline-flex items-center justify-center p-3 bg-zinc-900 rounded-2xl mb-6 shadow-xl border border-zinc-800">
              <currentMode.icon size={32} className="text-orange-400" />
            </div>
            <h2 className="text-4xl font-black text-white mb-4 tracking-tight">{currentMode.title}</h2>
            <p className="text-lg text-zinc-400 max-w-2xl leading-relaxed">{currentMode.desc}</p>
          </header>

          {/* Controls Grid */}
          <div className="grid grid-cols-1 md:grid-cols-2 gap-6 mb-12">

            {/* Speed Control */}
            {currentMode.hasSpeed && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <div className="flex items-center justify-between mb-6">
                  <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider">
                    <Activity size={16} className="text-orange-500" /> Speed
                  </label>
                  <span className="text-xs font-mono text-zinc-500 bg-zinc-950 px-2 py-1 rounded border border-zinc-800">
                    {tempSpeed}
                  </span>
                </div>
                <input
                  type="range"
                  min="1"
                  max="100"
                  value={tempSpeed}
                  onChange={(e) => setTempSpeed(parseInt(e.target.value))}
                  className="w-full h-2 bg-zinc-800 rounded-full appearance-none cursor-pointer accent-orange-500 hover:accent-orange-400 transition-all"
                />
                <div className="flex justify-between mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  <span>Slow</span>
                  <span>Fast</span>
                </div>
              </div>
            )}

            {/* Brightness Control */}
            {currentMode.hasBrightness && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <div className="flex items-center justify-between mb-6">
                  <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider">
                    <Sun size={16} className="text-orange-500" /> Brightness
                  </label>
                  <span className="text-xs font-mono text-zinc-500 bg-zinc-950 px-2 py-1 rounded border border-zinc-800">
                    {Math.round((tempBrightness / 255) * 100)}%
                  </span>
                </div>
                <input
                  type="range"
                  min="0"
                  max="255"
                  value={tempBrightness}
                  onChange={(e) => setTempBrightness(parseInt(e.target.value))}
                  className="w-full h-2 bg-zinc-800 rounded-full appearance-none cursor-pointer accent-orange-500 hover:accent-orange-400 transition-all"
                />
                <div className="flex justify-between mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  <span>Dim</span>
                  <span>Max</span>
                </div>
              </div>
            )}

            {/* Direction Control */}
            {currentMode.hasDirection && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-6">
                  <ArrowRightLeft size={16} className="text-orange-500" /> Direction
                </label>
                <div className="flex bg-zinc-950 p-1 rounded-xl border border-zinc-800">
                  <button
                    onClick={() => setDirection(1)}
                    className={clsx(
                      "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                      direction === 1
                        ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                        : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                    )}
                  >
                    Forward
                  </button>
                  <button
                    onClick={() => setDirection(0)}
                    className={clsx(
                      "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                      direction === 0
                        ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                        : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                    )}
                  >
                    Backward
                  </button>
                </div>
              </div>
            )}

            {/* Density / VAR1 Control */}
            {currentMode.hasDensity && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <div className="flex items-center justify-between mb-6">
                  <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider">
                    <Waves size={16} className="text-orange-500" /> Density
                  </label>
                  <span className="text-xs font-mono text-zinc-500 bg-zinc-950 px-2 py-1 rounded border border-zinc-800">
                    {tempVar1}
                  </span>
                </div>
                <input
                  type="range"
                  min="1"
                  max="50"
                  value={tempVar1}
                  onChange={(e) => setTempVar1(parseInt(e.target.value))}
                  className="w-full h-2 bg-zinc-800 rounded-full appearance-none cursor-pointer accent-orange-500 hover:accent-orange-400 transition-all"
                />
                <div className="flex justify-between mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  <span>Wide</span>
                  <span>Tight</span>
                </div>
              </div>
            )}
          </div>

          {/* Action Bar */}
          <div className="fixed bottom-0 right-0 p-8 w-[calc(100%-18rem)] bg-zinc-950/90 backdrop-blur-xl border-t border-zinc-800 flex items-center justify-between">
            <div>
              <div className="text-xs text-zinc-500 uppercase tracking-widest font-bold mb-1">Active Output</div>
              <div className="text-white font-medium flex items-center gap-2">
                <span className={`w-2 h-2 rounded-full ${activeMode === currentMode.id ? 'bg-orange-500 shadow-[0_0_8px_#f97316]' : 'bg-zinc-600'}`}></span>
                {activeMode === currentMode.id ? "Currently Running" : "Not Active"}
              </div>
            </div>
            <button
              onClick={() => handleApplyMode(currentMode.id)}
              disabled={activeMode === currentMode.id}
              className={clsx(
                "px-8 py-4 rounded-xl font-bold uppercase tracking-widest transition-all shadow-xl flex items-center gap-3",
                activeMode === currentMode.id
                  ? "bg-zinc-800 text-zinc-500 cursor-default border border-zinc-700"
                  : "bg-orange-500 text-white hover:bg-orange-400 hover:scale-[1.02] active:scale-[0.98] shadow-orange-900/20"
              )}
            >
              {activeMode === currentMode.id ? "Running" : "Activate Pattern"}
              {activeMode !== currentMode.id && <Zap size={18} />}
            </button>
          </div>

          {/* Spacer for bottom bar */}
          <div className="h-32"></div>

        </div>
      </main>
    </div>
  )
}

export default App
