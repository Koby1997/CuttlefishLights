import { useState, useEffect } from 'react';
import { serialService } from './services/SerialService';
import { Accordion } from './components/Accordion';
import {
  Zap, Power, Activity, Disc, AlignJustify,
  Palette, Music, Waves, ArrowRightLeft, Sun,
  LayoutDashboard, Settings
} from 'lucide-react';
import { clsx } from 'clsx';

function App() {
  const [connected, setConnected] = useState(false);
  const [activeMode, setActiveMode] = useState("OFF");
  const [expandedMode, setExpandedMode] = useState(null);

  // Temporary states for the controls inside accordions
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

  const handleApplyMode = async (modeId, hasSpeed, hasDirection, hasBrightness) => {
    if (!connected) return;

    // Speed Logic:
    // For RAINBOW: Speed is an increment (1-100), so higher is faster. Send raw value.
    // For OTHERS (Snake, etc): Speed is a delay, so higher slider should mean lower delay.
    let delay;
    if (modeId === "RAINBOW") {
      delay = tempSpeed; // Raw 1-100
    } else {
      // Invert for delay-based modes: 100 -> 5ms, 1 -> 100ms
      delay = hasSpeed ? (105 - tempSpeed) : 50;
    }

    // Direction: Default to 1 if not applicable
    const dirVal = hasDirection ? direction : 1;

    // Brightness: Default to 60 if not applicable (or global fallback?)
    // For now we use the temp value if the mode supports it, else we send the current temp value (so it doesn't jump?)
    // Actually, if 'hasBrightness' is false (like Switch on Beat), maybe we shouldn't touch it?
    // But our protocol expects a value. Let's send the existing tempBrightness so it stays same.
    const brightVal = tempBrightness;

    // VAR1 / VAR2
    const var1 = tempVar1;
    const var2 = 0;

    // Send the Single Unified Command
    // Updated Protocol: MODE, SPEED, DIR, BRIGHTNESS, VAR1, VAR2
    await serialService.sendConfig(modeId, delay, dirVal, brightVal, var1, var2);
  };

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
      hasBrightness: false // Disabled as requested
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

  return (
    <div className="flex h-screen bg-zinc-900 text-zinc-200 font-sans overflow-hidden">

      {/* SIDEBAR */}
      <aside className="w-80 bg-zinc-900 border-r border-zinc-800 flex flex-col z-20 shadow-2xl">

        {/* Header */}
        <div className="p-8 pb-4">
          <div className="flex items-center gap-3 mb-2">
            <div className="w-8 h-8 bg-orange-500 rounded-lg flex items-center justify-center shadow-lg shadow-orange-900/10">
              <LayoutDashboard className="text-white" size={20} />
            </div>
            <h1 className="text-xl font-black tracking-tight text-white">
              Cuttlefish<span className="text-orange-400">Ctrl</span>
            </h1>
          </div>
          <p className="text-xs text-zinc-500 font-mono uppercase tracking-widest pl-11">
            v2.1 // Industrial Soft
          </p>
        </div>

        {/* Global Controls Container */}
        <div className="flex-1 overflow-y-auto p-8 space-y-8">

          {/* Connection Status */}
          <div className="bg-zinc-800/50 p-4 rounded-xl border border-zinc-700">
            <div className="flex items-center justify-between mb-4">
              <span className="text-xs font-bold uppercase text-zinc-500 tracking-wider">Status</span>
              <div className={`w-2 h-2 rounded-full ${connected ? 'bg-orange-400 shadow-[0_0_8px_#fb923c]' : 'bg-red-400'}`} />
            </div>
            <button
              onClick={connected ? () => window.location.reload() : handleConnect}
              className={clsx(
                "w-full py-3 rounded-lg font-bold text-sm transition-all border",
                connected
                  ? "bg-zinc-700 text-zinc-300 border-zinc-600 hover:bg-zinc-600"
                  : "bg-orange-500 text-white border-transparent hover:bg-orange-400 shadow-lg shadow-orange-900/20"
              )}
            >
              {connected ? "Disconnect" : "Initialize Link"}
            </button>
          </div>

          {/* NOTE: Global Brightness Removed from Sidebar */}

          {/* Device Info / Footer */}
          <div className="mt-auto pt-8 border-t border-zinc-800">
            <div className="flex items-center gap-2 text-zinc-500 text-xs">
              <Settings size={12} />
              <span>Connected to: {connected ? "COM Port (WebSerial)" : "Wait..."}</span>
            </div>
          </div>
        </div>
      </aside>

      {/* MAIN CONTENT Area */}
      <main className="flex-1 bg-zinc-900 overflow-y-auto scrollbar-thin scrollbar-thumb-zinc-700 scrollbar-track-transparent">
        <div className="max-w-3xl mx-auto p-12">

          <div className="flex items-center justify-between mb-8">
            <div>
              <h2 className="text-2xl font-bold text-white mb-1">Lighting Protocol</h2>
              <p className="text-zinc-500">Select and configure active behavior pattern.</p>
            </div>
            <span className="bg-zinc-800 px-3 py-1 rounded-full text-xs font-mono text-zinc-500 border border-zinc-700">
              {modes.length} Modes Available
            </span>
          </div>

          <div className="space-y-4">
            {modes.map((mode) => (
              <Accordion
                key={mode.id}
                title={mode.title}
                description={mode.desc}
                icon={mode.icon}
                isActive={activeMode === mode.id}
                isOpen={expandedMode === mode.id}
                onToggle={() => setExpandedMode(expandedMode === mode.id ? null : mode.id)}
                onApply={() => handleApplyMode(mode.id, mode.hasSpeed, mode.hasDirection, mode.hasBrightness)}
              >
                {/* Internal Controls for this Mode */}
                <div className="grid grid-cols-1 md:grid-cols-2 gap-4">

                  {mode.hasBrightness && (
                    <div className="bg-zinc-800/50 backdrop-blur-md p-4 rounded-lg border border-zinc-700">
                      <label className="text-xs font-bold text-zinc-400 uppercase tracking-wider mb-3 block flex items-center gap-2">
                        <Sun size={14} className="text-orange-400" /> Brightness
                      </label>
                      <input
                        type="range"
                        min="0"
                        max="255"
                        value={tempBrightness}
                        onChange={(e) => setTempBrightness(parseInt(e.target.value))}
                        className="w-full h-1.5 bg-zinc-700 rounded-lg appearance-none cursor-pointer accent-orange-400"
                      />
                      <div className="flex justify-between mt-2 text-[10px] text-zinc-500 font-mono uppercase">
                        <span>Dim</span>
                        <span className="text-zinc-300">{Math.round((tempBrightness / 255) * 100)}%</span>
                        <span>Max</span>
                      </div>
                    </div>
                  )}

                  {mode.hasSpeed && (
                    <div className="bg-zinc-800/50 backdrop-blur-md p-4 rounded-lg border border-zinc-700">
                      <label className="text-xs font-bold text-zinc-400 uppercase tracking-wider mb-3 block flex items-center gap-2">
                        <Activity size={14} className="text-orange-400" /> Speed
                      </label>
                      <input
                        type="range"
                        min="1"
                        max="100"
                        value={tempSpeed}
                        onChange={(e) => setTempSpeed(parseInt(e.target.value))}
                        className="w-full h-1.5 bg-zinc-700 rounded-lg appearance-none cursor-pointer accent-orange-400"
                      />
                      <div className="flex justify-between mt-2 text-[10px] text-zinc-500 font-mono uppercase">
                        <span>Slow</span>
                        <span className="text-zinc-300">{tempSpeed}</span>
                        <span>Fast</span>
                      </div>
                    </div>
                  )}

                  {mode.hasDirection && (
                    <div className="bg-zinc-800/50 backdrop-blur-md p-4 rounded-lg border border-zinc-700">
                      <label className="text-xs font-bold text-zinc-400 uppercase tracking-wider mb-3 block flex items-center gap-2">
                        <ArrowRightLeft size={14} className="text-orange-400" /> Direction
                      </label>
                      <div className="flex bg-zinc-700 p-1 rounded-lg">
                        <button
                          onClick={() => setDirection(1)}
                          className={`flex-1 py-1.5 rounded-md text-xs font-bold transition-all ${direction === 1 ? 'bg-orange-500 text-white shadow-sm' : 'text-zinc-500 hover:text-zinc-300'}`}
                        >
                          Forward
                        </button>
                        <button
                          onClick={() => setDirection(0)}
                          className={`flex-1 py-1.5 rounded-md text-xs font-bold transition-all ${direction === 0 ? 'bg-orange-500 text-white shadow-sm' : 'text-zinc-500 hover:text-zinc-300'}`}
                        >
                          Backward
                        </button>
                      </div>
                    </div>
                  )}

                  {mode.hasDensity && (
                    <div className="bg-zinc-800/50 backdrop-blur-md p-4 rounded-lg border border-zinc-700">
                      <label className="text-xs font-bold text-zinc-400 uppercase tracking-wider mb-3 block flex items-center gap-2">
                        <Waves size={14} className="text-orange-400" /> Density (Rainbow Zoom)
                      </label>
                      <input
                        type="range"
                        min="1"
                        max="50"
                        value={tempVar1}
                        onChange={(e) => setTempVar1(parseInt(e.target.value))}
                        className="w-full h-1.5 bg-zinc-700 rounded-lg appearance-none cursor-pointer accent-orange-400"
                      />
                      <div className="flex justify-between mt-2 text-[10px] text-zinc-500 font-mono uppercase">
                        <span>Wide</span>
                        <span className="text-zinc-300">{tempVar1}</span>
                        <span>Tight</span>
                      </div>
                    </div>
                  )}

                  {/* Empty state if no controls */}
                  {!mode.hasSpeed && !mode.hasDirection && !mode.hasBrightness && !mode.hasDensity && (
                    <div className="col-span-2 text-center py-4 border border-dashed border-slate-800 rounded-lg">
                      <span className="text-xs text-slate-600 italic">No adjustable parameters for this mode.</span>
                    </div>
                  )}

                </div>
              </Accordion>
            ))}
          </div>

        </div>
      </main>
    </div>
  )
}

export default App
