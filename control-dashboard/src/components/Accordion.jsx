import React from 'react';
import { ChevronDown, Play, Circle } from 'lucide-react';
import { clsx } from 'clsx';
import { twMerge } from 'tailwind-merge';

export function Accordion({
    title,
    description,
    isActive,
    isOpen,
    onToggle,
    onApply,
    children,
    icon: Icon
}) {
    return (
        <div className={twMerge(
            "border rounded-xl transition-all duration-300 overflow-hidden",
            isActive
                ? "bg-zinc-800/80 border-orange-400/50 shadow-[0_0_15px_-5px_rgba(251,146,60,0.2)]"
                : "bg-zinc-800/50 border-zinc-700 hover:border-zinc-600"
        )}>

            {/* Header */}
            <button
                onClick={onToggle}
                className="w-full flex items-center justify-between p-4 text-left group"
            >
                <div className="flex items-center gap-4">
                    <div className={clsx(
                        "p-2 rounded-lg transition-colors",
                        isActive ? "bg-orange-500/20 text-orange-400" : "bg-zinc-700 text-zinc-400 group-hover:text-zinc-300"
                    )}>
                        {Icon && <Icon size={20} />}
                    </div>
                    <div>
                        <h3 className={clsx(
                            "font-bold text-base transition-colors",
                            isActive ? "text-orange-400" : "text-zinc-200"
                        )}>
                            {title}
                        </h3>
                        {isActive && (
                            <span className="text-[10px] font-bold uppercase tracking-wider text-orange-400 flex items-center gap-1">
                                <Circle size={6} fill="currentColor" /> Active
                            </span>
                        )}
                    </div>
                </div>

                <ChevronDown
                    className={clsx(
                        "text-zinc-500 transition-transform duration-300",
                        isOpen && "rotate-180"
                    )}
                />
            </button>

            {/* Body - Using Grid transition trick for smooth height animation */}
            <div
                className={clsx(
                    "grid transition-[grid-template-rows] duration-300 ease-in-out border-zinc-700/50",
                    isOpen ? "grid-rows-[1fr] border-t bg-zinc-900/30" : "grid-rows-[0fr] border-none bg-transparent"
                )}
            >
                <div className="overflow-hidden">
                    <div className="p-4">
                        <p className="text-sm text-zinc-400 mb-6 leading-relaxed">
                            {description}
                        </p>

                        {/* Controls Container */}
                        <div className="space-y-4 mb-6">
                            {children}
                        </div>

                        {/* Action Footer */}
                        <div className="flex justify-end pt-4 border-t border-zinc-700/50">
                            <button
                                onClick={(e) => {
                                    e.stopPropagation();
                                    onApply();
                                }}
                                className={clsx(
                                    "flex items-center gap-2 px-6 py-2 rounded-lg font-bold text-sm transition-all",
                                    isActive
                                        ? "bg-orange-400/10 text-orange-400 border border-orange-400/50 cursor-default"
                                        : "bg-orange-500 hover:bg-orange-400 text-white shadow-lg shadow-orange-900/20 active:scale-95"
                                )}
                            >
                                {isActive ? 'Currently Active' : <><Play size={16} /> Apply Pattern</>}
                            </button>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
}
