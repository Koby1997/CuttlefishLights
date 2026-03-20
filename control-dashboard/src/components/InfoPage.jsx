import React from "react";
import { BookOpen, Zap, Activity, HardDrive, AlertTriangle, ShieldAlert, Rocket, Waves, Link2, Disc } from "lucide-react";

export default function InfoPage() {
  return (
    <div className="flex-1 overflow-y-auto bg-zinc-950 text-zinc-300 p-8 pt-10 scroller-hide font-sans h-full">
      <style>{`
        @keyframes eq-b0 { 0%, 100% { height: 15%; } 25% { height: 80%; } 50% { height: 35%; } 75% { height: 95%; } }
        @keyframes eq-b1 { 0%, 100% { height: 30%; } 25% { height: 40%; } 50% { height: 100%; } 75% { height: 60%; } }
        @keyframes eq-b2 { 0%, 100% { height: 50%; } 25% { height: 90%; } 50% { height: 20%; } 75% { height: 70%; } }
        @keyframes eq-b3 { 0%, 100% { height: 25%; } 25% { height: 65%; } 50% { height: 95%; } 75% { height: 45%; } }
        @keyframes eq-b4 { 0%, 100% { height: 80%; } 25% { height: 30%; } 50% { height: 75%; } 75% { height: 40%; } }
        @keyframes eq-b5 { 0%, 100% { height: 60%; } 25% { height: 95%; } 50% { height: 50%; } 75% { height: 85%; } }
        @keyframes eq-b6 { 0%, 100% { height: 20%; } 25% { height: 60%; } 50% { height: 85%; } 75% { height: 30%; } }
        
        .eq-bar-0 { animation: eq-b0 2.8s ease-in-out infinite; }
        .eq-bar-1 { animation: eq-b1 3.4s ease-in-out infinite 0.1s; }
        .eq-bar-2 { animation: eq-b2 3.1s ease-in-out infinite 0.3s; }
        .eq-bar-3 { animation: eq-b3 3.6s ease-in-out infinite 0.2s; }
        .eq-bar-4 { animation: eq-b4 2.9s ease-in-out infinite 0.4s; }
        .eq-bar-5 { animation: eq-b5 3.8s ease-in-out infinite; }
        .eq-bar-6 { animation: eq-b6 3.2s ease-in-out infinite 0.1s; }
      `}</style>
      <div className="max-w-4xl mx-auto space-y-12 pb-20">

        {/* Header Title */}
        <div>
          <div className="flex items-center gap-4 mb-4">
            <div className="w-12 h-12 bg-zinc-800 rounded-xl flex items-center justify-center border border-zinc-700 shadow-md">
              <BookOpen className="text-orange-400" size={24} />
            </div>
            <h1 className="text-4xl font-black text-white tracking-tight">System Documentation</h1>
          </div>
          <p className="text-zinc-400 text-lg leading-relaxed max-w-2xl">
            A comprehensive guide to the physics, math, and hardware powering the Cuttlefish Lights engine.
          </p>
        </div>

        {/* High-Level ELI5 Overview */}
        <section className="bg-gradient-to-br from-orange-500/10 to-transparent border border-orange-500/20 rounded-2xl p-8 shadow-xl">
          <div className="flex items-center gap-3 mb-4">
            <Rocket className="text-orange-400" size={24} />
            <h2 className="text-2xl font-bold text-white">How This Actually Works</h2>
          </div>
          <p className="text-zinc-300 leading-relaxed text-lg">
            At its core, this project does three simple things incredibly fast:
          </p>
          <ul className="mt-4 space-y-3 text-zinc-400 list-disc list-inside">
            <li><strong className="text-zinc-200">1. Listening:</strong> A tiny microchip constantly listens to the music coming through the aux cord and slices that sound into 7 different "bands" (like splitting up the deep bass, the vocals, and the high crispy cymbals).</li>
            <li><strong className="text-zinc-200">2. Thinking:</strong> The "brain" (the Arduino) looks at each band to figure out what's happening in the song. It runs math algorithms to automatically handle super quiet songs or blaring loud drops.</li>
            <li><strong className="text-zinc-200">3. Painting:</strong> Based on the behavior you choose on this dashboard, the brain takes that band data and paints beautiful colors and movement down a giant string of LEDs!</li>
          </ul>

          <div className="mt-8 pt-8 border-t border-orange-500/20">
            <h3 className="flex items-center gap-2 text-xl font-bold text-white mb-3">
              <Waves className="text-orange-400" size={20} />
              Moving Sensitivity (The Volume Fix)
            </h3>
            <p className="text-zinc-400 leading-relaxed mb-4">
              When originally creating this project, I realized that the volume of the computer greatly affected the lights and I did not like that. I didn't want to have to blast my volume to have the lights catch all of the bass notes or fully light up with some of the behaviors.
            </p>
            <p className="text-zinc-400 leading-relaxed mb-4">
              My solution was to write a custom <strong className="text-zinc-200">Moving Sensitivity</strong> algorithm. The Arduino continually tracks an <strong>Exponential Moving Average</strong> (EMA) of the volume over a rapid flowing window of about <strong className="text-zinc-200">1 to 2 seconds</strong>. Because of this rapid moving math, the lights better react to the music no matter the volume, which also helps highlight unique notes.
            </p>
            <p className="text-zinc-400 leading-relaxed">
              Granted, this isn't a perfect solution, and the higher the volume on your computer, the better some of the behaviors work (I tend to just keep my computer volume at <strong className="text-zinc-200">100%</strong> to get the maximum sensitivity). However, the physical volume knob on your speakers doesn't matter. The Arduino only analyzes the raw digital signal leaving your computer's aux output. This means you can turn your loud speakers down to a whisper while keeping the computer volume turned up for flawless, full-range light reactions.
            </p>
          </div>

        </section>

        {/* The Audio Pipeline */}
        <section className="bg-zinc-900 border border-zinc-800 rounded-2xl p-8 shadow-xl">
          <div className="flex items-center gap-3 mb-6">
            <Activity className="text-zinc-400" size={24} />
            <h2 className="text-2xl font-bold text-white">The MSGEQ7 Audio Pipeline</h2>
          </div>

          <div className="prose prose-invert prose-zinc max-w-none">
            <p className="text-sm text-zinc-400 leading-relaxed mb-6">
              The Spectrum Shield uses a dedicated hardware chip (the MSGEQ7) to physically chop the incoming analog stereo aux signal into 7 distinct frequency bands utilizing internal analog band-pass filters. Each of these 7 bands peaks at a specific center frequency, capturing the entire spectrum of human hearing. The Arduino rapidly reads the amplitude height of each band hundreds of times per second.
            </p>

            <div className="grid grid-cols-7 gap-2 mb-2">
              {[
                { hz: '63Hz', color: 'text-red-500', bg: 'bg-red-500' },
                { hz: '160Hz', color: 'text-orange-500', bg: 'bg-orange-500' },
                { hz: '400Hz', color: 'text-yellow-500', bg: 'bg-yellow-500' },
                { hz: '1kHz', color: 'text-green-500', bg: 'bg-green-500' },
                { hz: '2.5kHz', color: 'text-blue-500', bg: 'bg-blue-500' },
                { hz: '6.25kHz', color: 'text-purple-500', bg: 'bg-purple-500' },
                { hz: '16kHz', color: 'text-pink-500', bg: 'bg-pink-500' }
              ].map((band, i) => (
                <div key={i} className="bg-zinc-950 border border-zinc-800 p-3 pt-4 rounded-xl flex flex-col items-center justify-between text-center overflow-hidden relative h-32 group hover:border-zinc-700 transition-colors">

                  {/* EQ Visualizer Background */}
                  <div className="absolute bottom-0 w-full h-full flex justify-center items-end opacity-20 group-hover:opacity-40 transition-opacity">
                    <div className={`w-full rounded-t-sm ${band.bg} eq-bar-${i} origin-bottom`}></div>
                  </div>

                  <span className={`text-xs font-black z-10 ${band.color}`}>BAND {i}</span>
                  <span className="text-[10px] text-zinc-300 font-bold tracking-tight z-10 mt-auto">{band.hz}</span>
                </div>
              ))}
              <div className="col-span-7 flex justify-between text-[11px] text-zinc-500/70 uppercase font-black tracking-widest px-2 mt-2">
                <span>Sub-Bass / Kicks</span>
                <span>Mid-Range / Vocals</span>
                <span>Hi-Hats / Air</span>
              </div>
            </div>
          </div>
        </section>

        {/* Hardware & Power Injection */}
        <section className="bg-zinc-900 border border-zinc-800 rounded-2xl p-8 shadow-xl">
          <div className="flex items-center gap-3 mb-6">
            <HardDrive className="text-zinc-400" size={24} />
            <h2 className="text-2xl font-bold text-white">Hardware & Power Physics</h2>
          </div>

          <div className="grid gap-6 md:grid-cols-2">
            <div className="bg-zinc-950/50 p-6 rounded-xl border border-zinc-800/80">
              <h3 className="flex items-center gap-2 text-lg font-bold text-orange-400 mb-3">
                <AlertTriangle size={18} /> Voltage Drop (The "Red" Problem)
              </h3>
              <p className="text-sm text-zinc-400 leading-relaxed mb-4">
                If you set the strip to pure White at 100% brightness, the lights at the far end of the strip will slowly turn yellow, orange, and eventually red. This is called <span className="text-zinc-200 font-bold">Voltage Drop</span>.
              </p>
              <p className="text-sm text-zinc-400 leading-relaxed">
                The tiny copper traces inside the strip act like resistors, eating up the 5V power as it travels down the line. Blue and Green LEDs require ~3.2V to turn on, while Red LEDs only need ~2.0V. When voltage drops too low at the end of the strip, only the Red LEDs survive!
              </p>
              {/* Voltage Drop Visualization */}
              <div className="my-6">
                <div className="flex h-4 rounded-full overflow-hidden w-full shadow-inner bg-zinc-950 border border-zinc-800">
                  <div className="w-1/4 h-full bg-white shadow-[0_0_10px_#ffffff]"></div>
                  <div className="w-1/4 h-full bg-gradient-to-r from-white to-yellow-300"></div>
                  <div className="w-1/4 h-full bg-gradient-to-r from-yellow-300 to-orange-500"></div>
                  <div className="w-1/4 h-full bg-gradient-to-r from-orange-500 to-red-600"></div>
                </div>
                <div className="flex justify-between text-[10px] font-bold text-zinc-500 mt-2 px-1 tracking-widest uppercase">
                  <span>Power Source (5V)</span>
                  <span>End of Strip (2V)</span>
                </div>
              </div>

              <div className="mt-4 p-3 bg-orange-950/30 border border-orange-900/50 rounded-xl shadow-inner">
                <span className="text-xs font-bold text-orange-400 uppercase tracking-widest block mb-1">The Fix: Power Injection</span>
                <p className="text-xs text-orange-200/70">You don't necessarily need a second power supply! If your main 5V supply has enough total Amps, simply run a thick wire directly from the power supply to the <strong>far end</strong> of the strip. This hits the starving LEDs with fresh 5V without forcing it through the internal copper traces.</p>
              </div>
            </div>

            <div className="bg-zinc-950/50 p-6 rounded-xl border border-zinc-800/80">
              <h3 className="flex items-center gap-2 text-lg font-bold text-sky-400 mb-3">
                <ShieldAlert size={18} /> The 1000uF Capacitor
              </h3>
              <p className="text-sm text-zinc-400 leading-relaxed mb-4">
                While the system theoretically runs without one, omitting the 1000uF capacitor across the power lines is dangerous for the LEDs.
              </p>
              <p className="text-sm text-zinc-400 leading-relaxed">
                When you plug a massive 10A or 15A power supply into a wall outlet, an enormous surge of "inrush current" instantly floods the wires. The WS2812B LEDs are incredibly delicate microchips; this initial surge can easily fry the very first pixel on your strip, permanently shattering the data line for all subsequent lights. The capacitor absorbs this explosive shockwave like an airbag.
              </p>
            </div>
          </div>
        </section>

        {/* Hardware Hook-up Guide */}
        <section className="bg-zinc-900 border border-zinc-800 rounded-2xl p-8 shadow-xl">
          <div className="flex items-center gap-3 mb-6">
            <Link2 className="text-zinc-400" size={24} />
            <h2 className="text-2xl font-bold text-white">Hardware Wiring Schematic</h2>
          </div>

          <p className="text-sm text-zinc-400 leading-relaxed mb-8">
            This is the master connection schematic for the Cuttlefish Lights hardware. If you are tearing down the project or rebuilding it, follow these exact wire mappings to ensure data runs correctly without frying components.
          </p>

          <div className="w-full bg-zinc-950 rounded-xl border border-zinc-800 p-8 mb-8 flex justify-center items-center overflow-x-auto shadow-inner hidden md:flex">
            <svg width="600" height="300" viewBox="0 0 600 300" className="drop-shadow-2xl">
              {/* Wires */}

              {/* 5V Power (Red) -> Strip */}
              <path d="M 120 100 L 480 100" stroke="#f43f5e" strokeWidth="4" fill="none" strokeDasharray="8 6" className="animate-[pulse_1.5s_linear_infinite]" />
              <text x="300" y="90" fill="#f43f5e" fontSize="12" fontWeight="bold" textAnchor="middle">5V (Red Wire)</text>

              {/* GND Power (Black) -> Strip */}
              <path d="M 120 130 L 480 130" stroke="#71717a" strokeWidth="4" fill="none" />
              <text x="200" y="148" fill="#71717a" fontSize="12" fontWeight="bold" textAnchor="middle">GND (Black Wire)</text>

              {/* GND Strip -> Arduino (Common Ground) */}
              <path d="M 500 160 L 500 210 L 370 210" stroke="#71717a" strokeWidth="4" fill="none" />
              <text x="440" y="228" fill="#71717a" fontSize="12" fontWeight="bold" textAnchor="middle">Common GND</text>

              {/* Data Arduino (Pin 6) -> Strip */}
              <path d="M 370 190 L 480 190 L 480 160" stroke="#10b981" strokeWidth="4" fill="none" />
              <circle cx="370" cy="190" r="4" fill="#10b981" />
              <text x="430" y="182" fill="#10b981" fontSize="12" fontWeight="bold" textAnchor="middle">Data (Pin 6)</text>

              {/* Audio Splitter Topology */}

              {/* Computer Box */}
              <rect x="20" y="250" width="100" height="40" rx="4" fill="#18181b" stroke="#3b82f6" strokeWidth="2" />
              <text x="70" y="275" fill="#ffffff" fontSize="12" fontWeight="bold" textAnchor="middle">Computer</text>

              {/* Physical Speakers Box */}
              <rect x="480" y="250" width="100" height="40" rx="4" fill="#18181b" stroke="#3b82f6" strokeWidth="2" />
              <text x="530" y="275" fill="#ffffff" fontSize="12" fontWeight="bold" textAnchor="middle">Speakers</text>

              {/* Splitter Node */}
              <circle cx="300" cy="270" r="16" fill="#18181b" stroke="#3b82f6" strokeWidth="2" />
              <text x="300" y="275" fill="#ffffff" fontSize="14" fontWeight="bold" textAnchor="middle">Y</text>
              <text x="300" y="298" fill="#3b82f6" fontSize="10" fontWeight="bold" textAnchor="middle">Aux Splitter</text>

              {/* Wires */}
              <path d="M 120 270 L 284 270" stroke="#3b82f6" strokeWidth="3" fill="none" />
              <path d="M 316 270 L 480 270" stroke="#3b82f6" strokeWidth="3" fill="none" />
              <path d="M 300 254 L 300 240" stroke="#3b82f6" strokeWidth="3" fill="none" />
              <circle cx="300" cy="240" r="4" fill="#3b82f6" />


              {/* Nodes (Hardware Components) */}

              {/* Power Supply Box */}
              <rect x="20" y="70" width="100" height="80" rx="8" fill="#18181b" stroke="#3f3f46" strokeWidth="2" />
              <text x="70" y="105" fill="#ffffff" fontSize="14" fontWeight="bold" textAnchor="middle">5V Power</text>
              <text x="70" y="125" fill="#a1a1aa" fontSize="12" textAnchor="middle">Supply (10A)</text>

              {/* Arduino Uno Box */}
              <rect x="230" y="160" width="140" height="80" rx="8" fill="#18181b" stroke="#3f3f46" strokeWidth="2" />
              <text x="300" y="195" fill="#ffffff" fontSize="14" fontWeight="bold" textAnchor="middle">Arduino Uno</text>
              <text x="300" y="215" fill="#a1a1aa" fontSize="12" textAnchor="middle">+ MSGEQ7 Shield</text>

              {/* LED Strip Box */}
              <rect x="480" y="70" width="100" height="90" rx="8" fill="#18181b" stroke="#3f3f46" strokeWidth="2" />
              <text x="530" y="110" fill="#ffffff" fontSize="14" fontWeight="bold" textAnchor="middle">LED Strip</text>
              <text x="530" y="130" fill="#a1a1aa" fontSize="12" textAnchor="middle">(WS2812B)</text>
            </svg>
          </div>

          <div className="grid gap-4">
            {/* Power Supply */}
            <div className="bg-zinc-950 p-4 rounded-xl border border-zinc-800 flex flex-col md:flex-row md:items-center gap-4">
              <div className="w-12 h-12 bg-rose-950/30 rounded-lg border border-rose-900/50 flex items-center justify-center shrink-0">
                <Zap className="text-rose-500" size={20} />
              </div>
              <div>
                <h4 className="text-white font-bold mb-1">5V Power Supply</h4>
                <p className="text-xs text-zinc-400">
                  <span className="text-rose-400 font-bold">RED wire</span> (VCC/5V) connects directly to the <span className="text-rose-400 font-bold">RED wire</span> of the LED strip.<br />
                  <span className="text-zinc-500 font-bold">BLACK wire</span> (Ground) connects directly to the <span className="text-zinc-500 font-bold">BLACK wire</span> (Ground) of the LED strip.
                </p>
              </div>
            </div>

            {/* LED Strip Data */}
            <div className="bg-zinc-950 p-4 rounded-xl border border-zinc-800 flex flex-col md:flex-row md:items-center gap-4">
              <div className="w-12 h-12 bg-emerald-950/30 rounded-lg border border-emerald-900/50 flex items-center justify-center shrink-0">
                <Activity className="text-emerald-500" size={20} />
              </div>
              <div>
                <h4 className="text-white font-bold mb-1">LED Strip Data & Ground</h4>
                <p className="text-xs text-zinc-400 leading-relaxed mt-1">
                  <span className="text-emerald-400 font-bold">DATA wire</span> (Green) connects to Arduino <strong className="text-white">Pin 6</strong>.<br />
                  <strong className="text-rose-500 uppercase tracking-widest text-[10px]">Critical:</strong> A secondary <span className="text-zinc-500 font-bold mt-1 inline-block">Ground wire</span> MUST branch off the LED strip and connect to any <strong className="text-white">GND Pin</strong> on the Arduino to complete the circuit loop, or the lights will glitch wildly!
                </p>
              </div>
            </div>

            {/* MSGEQ7 Spectrum Shield */}
            <div className="bg-zinc-950 p-4 rounded-xl border border-zinc-800 flex flex-col md:flex-row md:items-center gap-4">
              <div className="w-12 h-12 bg-sky-950/30 rounded-lg border border-sky-900/50 flex items-center justify-center shrink-0">
                <HardDrive className="text-sky-500" size={20} />
              </div>
              <div>
                <h4 className="text-white font-bold mb-1">MSGEQ7 Spectrum Shield</h4>
                <p className="text-xs text-zinc-400 leading-relaxed mt-1">
                  Simply snap the shield directly on top of the Arduino Uno headers like a hat. It automatically maps the audio to <strong className="text-white">A0</strong> and <strong className="text-white">A1</strong>, and uses <strong className="text-white">Pin 4</strong> (Strobe) and <strong className="text-white">Pin 5</strong> (Reset) to listen to the song.
                </p>
              </div>
            </div>

            {/* Audio Splitter & Speakers */}
            <div className="bg-zinc-950 p-4 rounded-xl border border-zinc-800 flex flex-col md:flex-row md:items-center gap-4">
              <div className="w-12 h-12 bg-blue-950/30 rounded-lg border border-blue-900/50 flex items-center justify-center shrink-0">
                <Disc className="text-blue-500" size={20} />
              </div>
              <div>
                <h4 className="text-white font-bold mb-1">Aux Splitter & Speakers</h4>
                <p className="text-xs text-zinc-400 leading-relaxed mt-1">
                  To listen to your music while driving the lights, attach a <strong className="text-white">Y-Splitter Aux cable</strong> to your computer's audio output. Run one wire to your <strong className="text-blue-400">physical speakers</strong>, and the other wire directly into the <strong className="text-blue-400">MSGEQ7 INPUT audio jack</strong>. This ensures the Arduino hears the exact same uncompressed raw signal your speakers do!
                </p>
              </div>
            </div>

          </div>
        </section>

      </div>
    </div>
  );
}
