import { useState, useEffect } from 'react';
import { serialService } from './services/SerialService';
import SortVisualizer from './components/SortVisualizer';
import {
  Zap, Power, Activity, Disc, AlignJustify,
  Palette, Music, Waves, Wind, ArrowRightLeft, Sun,
  LayoutDashboard, Settings, ChevronRight, Hammer,
  FastForward, Ruler, Blend, Paintbrush, Rocket, ChartColumn, Rainbow, Eclipse,
  AlignEndHorizontal
} from 'lucide-react';
import { clsx } from 'clsx';

function App() {
  const [connected, setConnected] = useState(false);

  // Modes Definition
  const modes = [
    {
      id: "RAINBOW",
      title: "Rainbow",
      icon: Rainbow,
      desc: "Smoothly cycling colors moving across the entire strip.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      hasDensity: true, // Mapped to VAR1
      category: "Ambient & Flow"
    },
    {
      id: "OG",
      title: "React: OG",
      icon: Music,
      desc: (
        <div className="space-y-2">
          <p>The original audio-reactive mode. Pulses with the beat.</p>
          <ul className="list-disc ml-5 text-sm text-zinc-500 font-mono">
            <li>Bass note = <span className="text-red-500 font-bold">Red</span></li>
            <li>Middle notes = <span className="text-green-500 font-bold">Green</span></li>
            <li>High notes = <span className="text-blue-500 font-bold">Blue</span></li>
            <li>No specific note hitting = <span className="text-yellow-500 font-bold">Yellow</span></li>
          </ul>
        </div>
      ),
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      category: "Music-Reactive"
    },
    {
      id: "SMOOTH",
      title: "Music Blend",
      icon: Blend,
      desc: "Bass drives red, mids drive green, highs drive blue — blended into a flowing color.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      category: "Music-Reactive"
    },
    {
      id: "SEVENCOLORS",
      title: "Seven Colors",
      icon: Palette,
      desc: (
        <div className="space-y-2">
          <p>Cycles through 7 distinct colors based on the dominant audio frequency.</p>
          <ul className="ml-5 text-sm text-zinc-500 font-mono list-disc">
            <li>Band 1 (Sub) = <span className="text-red-500 font-bold">Red</span></li>
            <li>Band 2 (Bass) = <span className="text-orange-500 font-bold">Orange</span></li>
            <li>Band 3 (Low Mid) = <span className="text-yellow-500 font-bold">Yellow</span></li>
            <li>Band 4 (Mid) = <span className="text-green-500 font-bold">Green</span></li>
            <li>Band 5 (High Mid) = <span className="text-blue-500 font-bold">Blue</span></li>
            <li>Band 6 (Treble) = <span className="text-purple-500 font-bold">Purple</span></li>
            <li>Band 7 (Air) = <span className="text-pink-500 font-bold">Pink</span></li>
          </ul>
        </div>
      ),
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      category: "Music-Reactive"
    },
    {
      id: "BOUNCE",
      title: "Seven Bounce",
      icon: AlignEndHorizontal,
      desc: "Bouncing balls of light that react to frequency bands. The Red section is the lower bass notes all the way up to Pink which are the high notes. Following the chart in Seven Colors.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: true,
      hasSubMode: true,  // 0 = Bounce, 1 = Fade
      hasResponse: true,  // Controls Attack smoothing speed (var2)
      hasDecay: true,    // Controls Decay gravity speed (var3)
      category: "Music-Reactive"
    },
    {
      id: "THREEBOUNCE",
      title: "Three Bounce",
      icon: AlignEndHorizontal,
      desc: "Like Seven Bounce, but groups the frequencies into 3 distinct Low, Mid, and High sections. The Low section represents bands 1-3, Mid represents bands 3-5, and High represents bands 5-7.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: true,
      hasSubMode: true,  // 0 = Bounce, 1 = Fade
      hasResponse: true,  // Controls Attack smoothing speed (var2)
      hasDecay: true,    // Controls Decay gravity speed (var3)
      hasSectionColors: true,
      category: "Music-Reactive"
    },
    {
      id: "MEGABOUNCE",
      title: "Mega Bounce",
      icon: Rocket,
      desc: "The entire strip acts as one massive, explosive volume meter.",
      hasSpeed: false,
      hasDirection: true, // Supports Forward, Backward, and Center-Out
      hasBrightness: true,
      hasSensitivityMode: true, // VAR1: 0=Bass(Punch), 1=All(Meter)
      hasStyleMode: true,       // VAR2: 0=Classic, 1=Fever, 2=Solid
      hasResponse: true,        // VAR3: Gravity smoothing
      category: "Music-Reactive"
    },

    {
      id: "SNAKE",
      title: "Snake",
      icon: Waves,
      desc: "A single 'snake' of light that slithers based on audio intensity.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      hasDensity: true, // Mapped to VAR1 (Snake Length)
      hasSnakeMode: true, // Mapped to VAR2 (Color Style)
      category: "Music-Reactive"
    },
    {
      id: "PAINTBRUSH",
      title: "Paintbrush",
      icon: Paintbrush,
      desc: "A single pointer that permanently paints an audio-reactive canvas. This is Similar to the Seven Colors behavior, but it changes how the lights are shown.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      hasSubMode: true, // Used for Typewriter vs Overwrite styles
      category: "Music-Reactive"
    },

    {
      id: "BASSNEW",
      title: "Bass Flow",
      icon: Activity, // Reusing Activity or Music icon
      desc: "Colors flow down the strip, changing every time the bass hits.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      category: "Music-Reactive"
    },
    {
      id: "SWITCH",
      title: "Switch On Beat",
      icon: Zap,
      desc: "Flashes the entire strip with a random color when a beat is detected.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: false,
      category: "Music-Reactive"
    },
    {
      id: "DRIFT",
      title: "Color Drift",
      icon: Eclipse,
      desc: "The whole strip slowly fades between random colors.",
      hasSpeed: true,
      hasDirection: false,
      hasBrightness: true,
      category: "Ambient & Flow"
    },
    {
      id: "FLOW",
      title: "Color Flow",
      icon: Wind,
      desc: "A smoothly transitioning color flows down the strip.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      category: "Ambient & Flow"
    },
    {
      id: "SORT",
      title: "Sort Visualizer",
      icon: ChartColumn,
      desc: "Watch sorting algorithms rearrange scrambled colors in real time.",
      hasSpeed: true,
      hasDirection: true,
      hasBrightness: true,
      hasSortMode: true,  // var1: 0=Bubble, 1=Selection, 2=Insertion, 3=Heap
      category: "Ambient & Flow"
    },
    {
      id: "SOLID",
      title: "Solid Color",
      icon: Sun,
      desc: "A solid, customizable static color of your choice.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: true,
      hasColorPicker: true,
      category: "Utilities"
    },
    {
      id: "OFF",
      title: "System Off",
      icon: Power,
      desc: "Turn off all LEDs.",
      hasSpeed: false,
      hasDirection: false,
      hasBrightness: false,
      category: "Utilities"
    }
  ];

  const [activeMode, setActiveMode] = useState("OFF"); // What is running on the device
  const [selectedMode, setSelectedMode] = useState(modes[0].id); // What is being viewed on UI

  // Temporary states for the controls
  const [tempSpeed, setTempSpeed] = useState(50); // 1-100
  const [direction, setDirection] = useState(1); // 1 = Forward, 0 = Backward
  const [tempBrightness, setTempBrightness] = useState(128); // 0-255
  const [tempVar1, setTempVar1] = useState(20); // Generic Variable 1
  const [tempVar2, setTempVar2] = useState(50); // Generic Variable 2 (e.g. Response)
  const [tempVar3, setTempVar3] = useState(0); // Generic Variable 3
  const [tempColor, setTempColor] = useState("#ffffff"); // Hex Color for Solid Color mode
  const [section1Color, setSection1Color] = useState("#ff0000"); // Red Default
  const [section2Color, setSection2Color] = useState("#00ff00"); // Green Default
  const [section3Color, setSection3Color] = useState("#ff00d2"); // Pink Default

  useEffect(() => {
    // Force the aesthetic defaults for the initially selected mode on page load
    handleModeSelect(modes[0].id);
  }, []);

  const handleModeSelect = (modeId) => {
    setSelectedMode(modeId);

    // Apply user-defined default aesthetics
    // (We will expand this map as we go through each mode with the user)
    switch (modeId) {
      case "OG":
        setTempSpeed(100);
        setDirection(0); // Backward
        setTempVar1(20);
        break;
      case "SMOOTH":
        setTempSpeed(100);
        setDirection(0); // Backward
        break;
      case "SEVENCOLORS":
        setTempSpeed(100);
        setDirection(0); // Backward
        setTempVar1(20);
        break;
      case "BOUNCE":
        setTempSpeed(50); // N/A
        setDirection(1); // N/A
        setTempVar1(0); // Style: Bounce
        setTempVar2(8); // Attack: 8 (Snappy default)
        setTempVar3(5); // Decay: 5 (Standard default)
        break;
      case "THREEBOUNCE":
        setTempSpeed(50); // N/A
        setDirection(1); // N/A
        setTempVar1(0); // Style: Bounce
        setTempVar2(8); // Attack
        setTempVar3(5); // Decay
        setSection1Color("#ff0000"); // Red Default
        setSection2Color("#00ff00"); // Green Default
        setSection3Color("#ff00d2"); // Pink Default
        break;
      case "MEGABOUNCE":
        setTempSpeed(50); // N/A
        setDirection(0); // Backward
        setTempVar1(0); // Sensitivity: Bass
        setTempVar2(0); // Style: Classic
        setTempVar3(1); // Response: 1
        break;
      case "SNAKE":
        setTempSpeed(100);
        setDirection(0); // Backward
        setTempVar1(50); // Length: 50
        setTempVar2(1); // Color Style: Rainbow
        break;
      case "PAINTBRUSH":
        setTempSpeed(100);
        setDirection(0); // Backward
        setTempVar1(0); // Style: Overwrite (formerly called Black Out / Color Over depending on state)
        break;
      case "BASSNEW":
        setTempSpeed(100);
        setDirection(0); // Backward
        break;
      case "SWITCH":
        setTempSpeed(50);
        setDirection(1);
        break;
      case "RAINBOW":
        setTempSpeed(15);
        setDirection(0); // Backward
        setTempVar1(20); // Density
        break;
      case "DRIFT":
        setTempSpeed(100);
        setDirection(1); // N/A
        break;
      case "FLOW":
        setTempSpeed(100);
        setDirection(0); // Backward
        break;
      case "SORT":
        setTempSpeed(100);
        setDirection(0); // Backward
        setTempVar1(0); // Algorithm: Bubble
        break;
      case "SOLID":
        setTempSpeed(50); // N/A
        setDirection(1); // N/A
        setTempColor("#ffffff");
        break;
      case "OFF":
        break;
      default:
        // Generic safe defaults
        setTempSpeed(50);
        setDirection(1);
        setTempVar1(0);
        setTempVar2(50);
        break;
    }
  };

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
    if (mode.id === "MEGABOUNCE") {
      delay = 1; // Locked to 100/max speed minimum delay for visualizer precision
    } else if (mode.id === "RAINBOW") {
      delay = tempSpeed; // Raw 1-100
    } else if (mode.id === "OG" || mode.id === "SEVENCOLORS" || mode.id === "SNAKE" || mode.id === "PAINTBRUSH") {
      delay = Math.max(1, 101 - tempSpeed);
    } else {
      // Invert for delay-based modes: 100 -> 5ms, 1 -> 100ms
      delay = mode.hasSpeed ? (105 - tempSpeed) : 50;
    }

    // Direction: Default to 1 if not applicable
    const dirVal = mode.hasDirection ? direction : 1;

    // Brightness: Default to 60 or tempBrightness
    const brightVal = tempBrightness;

    // VAR1 / VAR2 / VAR3
    let var1 = tempVar1;
    let var2 = tempVar2;
    let var3 = (mode.id === "MEGABOUNCE" || mode.id === "BOUNCE" || mode.id === "THREEBOUNCE") ? tempVar3 : 0;

    let r = 0, g = 0, b = 0;
    let r2 = 0, g2 = 0, b2 = 0;
    let r3 = 0, g3 = 0, b3 = 0;

    // For Solid Color Mode, intercept vars and send RGB
    if (mode.hasColorPicker) {
      const hex = tempColor.replace('#', '');
      var1 = parseInt(hex.substring(0, 2), 16); // R
      var2 = parseInt(hex.substring(2, 4), 16); // G
      var3 = parseInt(hex.substring(4, 6), 16); // B
      // Also set r, g, b just in case
      r = var1; g = var2; b = var3;
    } else if (mode.id === "MEGABOUNCE" && tempVar2 === 2) {
      const hex = tempColor.replace('#', '');
      r = parseInt(hex.substring(0, 2), 16);
      g = parseInt(hex.substring(2, 4), 16);
      b = parseInt(hex.substring(4, 6), 16);
    } else if (mode.id === "THREEBOUNCE") {
      const hex1 = section1Color.replace('#', '');
      r = parseInt(hex1.substring(0, 2), 16);
      g = parseInt(hex1.substring(2, 4), 16);
      b = parseInt(hex1.substring(4, 6), 16);

      const hex2 = section2Color.replace('#', '');
      r2 = parseInt(hex2.substring(0, 2), 16);
      g2 = parseInt(hex2.substring(2, 4), 16);
      b2 = parseInt(hex2.substring(4, 6), 16);

      const hex3 = section3Color.replace('#', '');
      r3 = parseInt(hex3.substring(0, 2), 16);
      g3 = parseInt(hex3.substring(2, 4), 16);
      b3 = parseInt(hex3.substring(4, 6), 16);
    }

    // Send the Single Unified Command
    await serialService.sendConfig(mode.id, delay, dirVal, brightVal, var1, var2, var3, r, g, b, r2, g2, b2, r3, g3, b3);
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
          {/* Group modes by category */}
          {["Music-Reactive", "Ambient & Flow", "Utilities"].map((category) => {
            const categoryModes = modes.filter((m) => m.category === category);
            if (categoryModes.length === 0) return null;

            return (
              <div key={category} className="mb-6">
                {category !== "Music-Reactive" && <hr className="border-zinc-800/60 mb-6 mx-2" />}
                <div className="px-2 mb-2 text-xs font-bold text-zinc-500 uppercase tracking-wider">
                  {category}
                </div>
                <div className="space-y-1">
                  {categoryModes.map((mode) => {
                    const isSelected = selectedMode === mode.id;
                    const isActive = activeMode === mode.id;
                    const Icon = mode.icon;

                    return (
                      <button
                        key={mode.id}
                        onClick={() => handleModeSelect(mode.id)}
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
              </div>
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
                    <FastForward size={16} className="text-orange-500" /> Speed
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
                  {currentMode.id === "MEGABOUNCE" && (
                    <button
                      onClick={() => setDirection(2)}
                      className={clsx(
                        "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                        direction === 2
                          ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                          : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                      )}
                    >
                      Center-Out
                    </button>
                  )}
                </div>
              </div>
            )}

            {/* Density / VAR1 Control */}
            {currentMode.hasDensity && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <div className="flex items-center justify-between mb-6">
                  <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider">
                    {currentMode.id === "SNAKE" ? (
                      <Ruler size={16} className="text-orange-500" />
                    ) : (
                      <Waves size={16} className="text-orange-500" />
                    )}
                    {currentMode.id === "SNAKE" ? "Snake Length" : "Density"}
                  </label>
                  <span className="text-xs font-mono text-zinc-500 bg-zinc-950 px-2 py-1 rounded border border-zinc-800">
                    {tempVar1}
                  </span>
                </div>
                <input
                  type="range"
                  min="1"
                  max="100"
                  value={tempVar1}
                  onChange={(e) => setTempVar1(parseInt(e.target.value))}
                  className="w-full h-2 bg-zinc-800 rounded-full appearance-none cursor-pointer accent-orange-500 hover:accent-orange-400 transition-all"
                />
                <div className="flex justify-between mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  {currentMode.id === "SNAKE" ? (
                    <>
                      <span>Shorter</span>
                      <span>Longer</span>
                    </>
                  ) : (
                    <>
                      <span>Wide</span>
                      <span>Tight</span>
                    </>
                  )}
                </div>
              </div>
            )}

            {/* Sensitivity Control (VAR1) */}
            {currentMode.hasSensitivityMode && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-6">
                  <Activity size={16} className="text-orange-500" /> Sensitivity
                </label>
                <div className="flex bg-zinc-950 p-1 rounded-xl border border-zinc-800">
                  <button
                    onClick={() => setTempVar1(0)}
                    className={clsx(
                      "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                      tempVar1 === 0
                        ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                        : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                    )}
                  >
                    Punch (Bass)
                  </button>
                  <button
                    onClick={() => setTempVar1(1)}
                    className={clsx(
                      "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                      tempVar1 === 1
                        ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                        : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                    )}
                  >
                    Meter (All)
                  </button>
                </div>
              </div>
            )}

            {/* Attack / Smoothing Control (VAR3 for MegaBounce, VAR2 for old modes) */}
            {currentMode.hasResponse && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <div className="flex items-center justify-between mb-6">
                  <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider">
                    <ChevronRight size={16} className="text-orange-500" /> Attack
                  </label>
                  <span className="text-xs font-mono text-zinc-500 bg-zinc-950 px-2 py-1 rounded border border-zinc-800">
                    {currentMode.id === "MEGABOUNCE" ? tempVar3 : tempVar2}
                  </span>
                </div>
                <input
                  type="range"
                  min="1"
                  max={(currentMode.id === "MEGABOUNCE" || currentMode.id === "BOUNCE" || currentMode.id === "THREEBOUNCE") ? "10" : "100"}
                  value={currentMode.id === "MEGABOUNCE" ? tempVar3 : tempVar2}
                  onChange={(e) => {
                    const val = parseInt(e.target.value);
                    if (currentMode.id === "MEGABOUNCE") setTempVar3(val);
                    else setTempVar2(val);
                  }}
                  className="w-full h-2 bg-zinc-800 rounded-full appearance-none cursor-pointer accent-orange-500 hover:accent-orange-400 transition-all"
                />
                <div className="flex justify-between mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  <span>Smooth</span>
                  <span>Snappy</span>
                </div>
              </div>
            )}

            {/* Decay / Gravity Control (VAR3 intentionally isolated for Bounce) */}
            {currentMode.hasDecay && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <div className="flex items-center justify-between mb-6">
                  <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider">
                    <ChevronRight size={16} className="text-orange-500 rotate-90" /> Decay
                  </label>
                  <span className="text-xs font-mono text-zinc-500 bg-zinc-950 px-2 py-1 rounded border border-zinc-800">
                    {tempVar3}
                  </span>
                </div>
                <input
                  type="range"
                  min="1"
                  max="10"
                  value={tempVar3}
                  onChange={(e) => setTempVar3(parseInt(e.target.value))}
                  className="w-full h-2 bg-zinc-800 rounded-full appearance-none cursor-pointer accent-orange-500 hover:accent-orange-400 transition-all"
                />
                <div className="flex justify-between mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  <span>Slow (Float)</span>
                  <span>Fast (Drop)</span>
                </div>
              </div>
            )}

            {/* Sub-Mode Toggle (e.g. Bounce / Fade) */}
            {currentMode.hasSubMode && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl">
                <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-6">
                  <Activity size={16} className="text-orange-500" /> Style
                </label>
                <div className="flex bg-zinc-950 p-1 rounded-xl border border-zinc-800">
                  <button
                    onClick={() => setTempVar1(0)}
                    className={clsx(
                      "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                      tempVar1 === 0
                        ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                        : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                    )}
                  >
                    {currentMode.id === "PAINTBRUSH" ? "Overwrite" : "Bounce"}
                  </button>
                  <button
                    onClick={() => setTempVar1(1)}
                    className={clsx(
                      "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                      tempVar1 === 1
                        ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                        : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                    )}
                  >
                    {currentMode.id === "PAINTBRUSH" ? "Black out" : "Fade"}
                  </button>
                </div>
              </div>
            )}

            {/* Style Control (VAR2) */}
            {currentMode.hasStyleMode && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl md:col-span-2">
                <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-6">
                  <Palette size={16} className="text-orange-500" /> Color Style
                </label>
                <div className="flex bg-zinc-950 p-1 rounded-xl border border-zinc-800 gap-1">
                  {[
                    { label: "Classic", val: 0 },
                    { label: "Custom Solid", val: 2 },
                    { label: "Full Spectrum", val: 3 }
                  ].map(({ label, val }) => (
                    <button
                      key={val}
                      onClick={() => setTempVar2(val)}
                      className={clsx(
                        "flex-1 py-3 rounded-lg text-[10px] md:text-xs font-bold transition-all uppercase tracking-wide",
                        tempVar2 === val
                          ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                          : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                      )}
                    >
                      {label}
                    </button>
                  ))}
                </div>
              </div>
            )}


            {/* Snake Color Style Selector (VAR2) */}
            {currentMode.hasSnakeMode && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl md:col-span-2">
                <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-6">
                  <Palette size={16} className="text-orange-500" /> Color Style
                </label>
                <div className="flex bg-zinc-950 p-1 rounded-xl border border-zinc-800 gap-1">
                  {[
                    { label: "Audio", val: 0 },
                    { label: "Rainbow", val: 1 },
                    { label: "Locked Bow", val: 2 },
                    { label: "Random", val: 3 }
                  ].map(({ label, val }) => (
                    <button
                      key={val}
                      onClick={() => setTempVar2(val)}
                      className={clsx(
                        "flex-1 py-3 rounded-lg text-[10px] md:text-xs font-bold transition-all uppercase tracking-wide",
                        tempVar2 === val
                          ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                          : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                      )}
                    >
                      {label}
                    </button>
                  ))}
                </div>
                <p className="mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  Changes how the snake determines its body color
                </p>
              </div>
            )}

            {/* Sort Algorithm Selector (VAR1) */}
            {currentMode.hasSortMode && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl md:col-span-2">
                <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-6">
                  <AlignJustify size={16} className="text-orange-500" /> Algorithm
                </label>
                <div className="flex bg-zinc-950 p-1 rounded-xl border border-zinc-800 gap-1">
                  {[
                    { label: "Bubble", val: 0 },
                    { label: "Selection", val: 1 },
                    { label: "Insertion", val: 2 },
                    { label: "Heap", val: 3 },
                  ].map(({ label, val }) => (
                    <button
                      key={val}
                      onClick={() => setTempVar1(val)}
                      className={clsx(
                        "flex-1 py-3 rounded-lg text-xs font-bold transition-all uppercase tracking-wide",
                        tempVar1 === val
                          ? "bg-zinc-800 text-white shadow-sm border border-zinc-700"
                          : "text-zinc-500 hover:text-zinc-300 hover:bg-zinc-900"
                      )}
                    >
                      {label}
                    </button>
                  ))}
                </div>
                <p className="mt-3 text-[10px] text-zinc-600 font-mono uppercase tracking-widest">
                  Direction sets ascending ↑ or descending ↓
                </p>

                {/* Visualizer Injection */}
                <SortVisualizer algorithm={tempVar1} direction={direction} speed={tempSpeed} />

              </div>
            )}

            {/* Section Colors for Three Bounce */}
            {currentMode.hasSectionColors && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl md:col-span-2">
                <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-6">
                  <Palette size={16} className="text-orange-500" /> Section Colors
                </label>

                <div className="space-y-4">
                  {[
                    { label: "Low Section", state: section1Color, setter: setSection1Color },
                    { label: "Mid Section", state: section2Color, setter: setSection2Color },
                    { label: "High Section", state: section3Color, setter: setSection3Color }
                  ].map((section, sIdx) => (
                    <div key={sIdx} className="mb-4 last:mb-0 flex items-center justify-between bg-zinc-950 p-4 rounded-xl border border-zinc-800">
                      <div>
                        <span className="text-sm font-bold text-zinc-300 block">{section.label}</span>
                        <span className="text-[10px] text-zinc-500 font-mono uppercase">Custom HTML RGB Wheel</span>
                      </div>
                      <div className="relative group cursor-pointer">
                        <input
                          type="color"
                          value={section.state}
                          onChange={(e) => section.setter(e.target.value)}
                          className="w-10 h-10 p-0 border-0 rounded-full overflow-hidden cursor-pointer"
                        />
                        <div className="absolute inset-0 ring-2 ring-zinc-700/50 rounded-full pointer-events-none group-hover:ring-orange-500/50 transition-all"></div>
                      </div>
                    </div>
                  ))}
                </div>
              </div>
            )}

            {/* Solid Color Picker */}
            {(currentMode.hasColorPicker || (currentMode.id === "MEGABOUNCE" && tempVar2 === 2)) && (
              <div className="bg-zinc-900/80 backdrop-blur-sm p-6 rounded-2xl border border-zinc-800 shadow-xl flex flex-col sm:flex-row items-center justify-between gap-6">
                <div>
                  <label className="flex items-center gap-2 text-sm font-bold text-zinc-300 uppercase tracking-wider mb-2">
                    <Palette size={16} className="text-orange-500" /> Strip Color
                  </label>
                  <p className="text-[10px] text-zinc-500 font-mono uppercase tracking-widest">
                    Select the exact RGB to paint the entire strip
                  </p>
                </div>
                <div className="relative group cursor-pointer">
                  <input
                    type="color"
                    value={tempColor}
                    onChange={(e) => {
                      const newColor = e.target.value;
                      setTempColor(newColor);

                      const hex = newColor.replace('#', '');
                      const r = parseInt(hex.substring(0, 2), 16);
                      const g = parseInt(hex.substring(2, 4), 16);
                      const b = parseInt(hex.substring(4, 6), 16);

                      // Instantly activate and blast color to the strip
                      if (currentMode.id === "MEGABOUNCE") {
                        serialService.sendConfig(currentMode.id, 50, direction, tempBrightness, tempVar1, tempVar2, tempVar3, r, g, b);
                      } else {
                        serialService.sendConfig(currentMode.id, 50, 1, tempBrightness, r, g, b, r, g, b);
                      }

                      if (activeMode !== currentMode.id) {
                        setActiveMode(currentMode.id);
                      }
                    }}
                    className="w-16 h-16 rounded-xl cursor-pointer bg-transparent border-0 p-0 shadow-lg shadow-black/40 overflow-hidden"
                  />
                  <div className="absolute inset-0 rounded-xl ring-2 ring-white/10 group-hover:ring-orange-500/50 transition-all pointer-events-none" />
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
              className={clsx(
                "px-8 py-4 rounded-xl font-bold uppercase tracking-widest transition-all shadow-xl flex items-center gap-3",
                "bg-orange-500 text-white hover:bg-orange-400 hover:scale-[1.02] active:scale-[0.98] shadow-orange-900/20"
              )}
            >
              {activeMode === currentMode.id ? "Update Pattern" : "Activate Pattern"}
              <Zap size={18} />
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
