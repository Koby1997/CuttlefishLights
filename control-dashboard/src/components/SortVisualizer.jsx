import React, { useState, useEffect, useRef } from 'react';
import { clsx } from 'clsx';
import { RefreshCw } from 'lucide-react';

const NUM_LEDS = 20;

export default function SortVisualizer({ algorithm, direction, speed: initialSpeed }) {
    const [leds, setLeds] = useState([]);
    const [activeIndices, setActiveIndices] = useState([]); // indices being compared/swapped
    const [sortedIndices, setSortedIndices] = useState([]); // indices fully sorted
    const [speed, setSpeed] = useState(initialSpeed || 50); // Local speed state for preview
    const stateRef = useRef({ array: [], i: 0, j: 0, done: false, phase: 0, k: 0 });
    const timerRef = useRef(null);

    const isAscending = direction === 1;

    // HSL helper (0-255 hue to 0-360 standard CSS HSL)
    const getHslStr = (hueVal) => `hsl(${(hueVal / 255) * 360}, 100%, 60%)`;

    const resetAndShuffle = () => {
        const newLeds = Array.from({ length: NUM_LEDS }, () => Math.floor(Math.random() * 255));
        setLeds(newLeds);
        setActiveIndices([]);
        setSortedIndices([]);
        stateRef.current = { array: [...newLeds], i: 0, j: 0, done: false, phase: 0, k: NUM_LEDS / 2 - 1 };
    };

    // Re-shuffle when the target algorithm or direction changes
    useEffect(() => {
        resetAndShuffle();
    }, [algorithm, direction]);

    useEffect(() => {
        if (stateRef.current.done) return;

        // Map visualizer speed to delay:
        // Slowest (1) = 300ms delay per tick
        // Fastest (100) = 15ms delay per tick
        const delayMs = Math.max(15, 305 - (speed * 2.9));

        timerRef.current = setInterval(() => {
            tickSort();
        }, delayMs);

        return () => clearInterval(timerRef.current);
    }, [algorithm, direction, speed, leds]); // Depend on speed state

    const tickSort = () => {
        const state = stateRef.current;
        if (state.done) return;

        let arr = [...state.array];   // Working copy
        let newActive = [];
        let newSorted = [...sortedIndices];

        const swap = (idxA, idxB) => {
            let tmp = arr[idxA];
            arr[idxA] = arr[idxB];
            arr[idxB] = tmp;
        };

        // --- BUBBLE SORT (algo 0) ---
        if (algorithm === 0) {
            if (state.i < NUM_LEDS - 1) {
                let swappedThisPass = false;
                if (state.j < NUM_LEDS - 1 - state.i) {
                    const a = state.j;
                    const b = state.j + 1;
                    newActive = [a, b];

                    if (isAscending ? (arr[a] > arr[b]) : (arr[a] < arr[b])) {
                        swap(a, b);
                    }
                    state.j++;
                } else {
                    // Pass finished, the element at (NUM_LEDS - 1 - state.i) is now sorted!
                    newSorted.push(NUM_LEDS - 1 - state.i);
                    state.i++;
                    state.j = 0;
                }
            } else {
                newSorted.push(0); // The final left element is sorted
                state.done = true;
            }
        }

        // --- SELECTION SORT (algo 1) ---
        else if (algorithm === 1) {
            if (state.i < NUM_LEDS - 1) {
                // We will speed up selection sort visualization by doing one full scan + swap per tick
                // (Matching the Arduino Arduino optimization)
                let target = state.i;
                for (let j = state.i + 1; j < NUM_LEDS; j++) {
                    if (isAscending ? (arr[j] < arr[target]) : (arr[j] > arr[target])) {
                        target = j;
                    }
                }
                if (target !== state.i) {
                    newActive = [state.i, target];
                    swap(state.i, target);
                } else {
                    newActive = [state.i];
                }
                newSorted.push(state.i);
                state.i++;
            } else {
                newSorted.push(NUM_LEDS - 1);
                state.done = true;
            }
        }

        // --- INSERTION SORT (algo 2) ---
        else if (algorithm === 2) {
            if (state.i < NUM_LEDS) {
                if (state.j === 0) {
                    // Start a new insertion
                    state.j = state.i;
                }

                if (state.j > 0 && (isAscending ? (arr[state.j - 1] > arr[state.j]) : (arr[state.j - 1] < arr[state.j]))) {
                    newActive = [state.j - 1, state.j];
                    swap(state.j - 1, state.j);
                    state.j--;
                } else {
                    // Insertion complete for this element
                    // Because we build from left, we can just say everything from 0 to i is fully "sorted" so far
                    newSorted = Array.from({ length: state.i + 1 }, (_, idx) => idx);
                    state.i++;
                    state.j = state.i; // Prep for next
                }
            } else {
                state.done = true;
            }
        }

        // --- HEAP SORT (algo 3) ---
        else if (algorithm === 3) {
            const maxHeap = isAscending;

            const heapifyIter = (n, root) => {
                let idx = root;
                while (true) {
                    let best = idx;
                    let left = 2 * idx + 1;
                    let right = 2 * idx + 2;

                    if (left < n && (maxHeap ? arr[left] > arr[best] : arr[left] < arr[best])) best = left;
                    if (right < n && (maxHeap ? arr[right] > arr[best] : arr[right] < arr[best])) best = right;

                    if (best === idx) break;
                    swap(idx, best);
                    newActive = [idx, best]; // Show what swapped
                    idx = best;
                }
            };

            if (state.phase === 0) {
                // Building Heap
                if (state.k >= 0) {
                    heapifyIter(NUM_LEDS, state.k);
                    state.k--;
                } else {
                    state.phase = 1;
                    state.k = NUM_LEDS - 1;
                }
            } else {
                // Extracting
                if (state.k > 0) {
                    // The root of the heap is the max/min. Swap it to the end where it belongs
                    newActive = [0, state.k];
                    swap(0, state.k);
                    newSorted.push(state.k); // This end spot is now permanently sorted!
                    heapifyIter(state.k, 0);
                    state.k--;
                } else {
                    newSorted.push(0); // The final spot is sorted
                    state.done = true;
                }
            }
        }

        // Apply state
        state.array = arr;
        setLeds(arr);
        setActiveIndices(newActive);
        setSortedIndices(newSorted);

        // If done, pause slightly then shuffle
        if (state.done) {
            clearInterval(timerRef.current);
            setTimeout(() => {
                resetAndShuffle();
            }, 1500); // 1.5 seconds delay before restart
        }
    };

    return (
        <div className="bg-zinc-950 p-4 rounded-xl border border-zinc-800 shadow-inner mt-4">
            <div className="flex justify-between items-center mb-4">
                <span className="text-xs font-bold text-zinc-500 uppercase tracking-widest flex items-center gap-2">
                    Live Preview
                </span>
                <button
                    onClick={resetAndShuffle}
                    className="text-zinc-500 hover:text-orange-400 transition-colors"
                    title="Restart Simulation"
                >
                    <RefreshCw size={14} />
                </button>
            </div>

            <div className="flex items-end justify-between h-20 gap-[2px]">
                {leds.map((hue, idx) => {
                    const isActive = activeIndices.includes(idx);
                    const isSorted = sortedIndices.includes(idx);

                    return (
                        <div
                            key={idx}
                            className={clsx(
                                "flex-1 rounded-sm transition-all duration-75",
                                isActive ? "brightness-150 scale-110 z-10" : "",
                                isSorted ? "opacity-100" : "opacity-60"
                            )}
                            style={{
                                backgroundColor: getHslStr(hue),
                                height: `${(hue / 255) * 100}%`,
                                minHeight: '10%' // Make sure completely dark colors still have a little bar
                            }}
                        />
                    );
                })}
            </div>
            <div className="mt-2 mb-4 text-[9px] text-zinc-700 font-mono text-center uppercase">
                {stateRef.current.done ? 'Sorted. Restarting...' : 'Sorting in progress...'}
            </div>

            <div className="border-t border-zinc-900 pt-3 mt-1">
                <div className="flex justify-between items-center mb-2">
                    <span className="text-[10px] font-bold text-zinc-600 uppercase tracking-wider">Preview Speed</span>
                    <span className="text-[10px] font-mono text-zinc-500">{speed}</span>
                </div>
                <input
                    type="range"
                    min="1"
                    max="100"
                    value={speed}
                    onChange={(e) => setSpeed(parseInt(e.target.value))}
                    className="w-full h-1 bg-zinc-800 rounded-full appearance-none cursor-pointer accent-zinc-500 hover:accent-zinc-400 transition-all"
                />
            </div>
        </div>
    );
}
