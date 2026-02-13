import { clsx } from 'clsx';
import { twMerge } from 'tailwind-merge';

export function ModeButton({ active, children, onClick, color = "blue" }) {
    const baseStyles = "px-6 py-4 rounded-xl font-bold transition-all duration-200 shadow-lg hover:scale-105 active:scale-95 border border-white/10 backdrop-blur-md";

    const colorStyles = {
        blue: active
            ? "bg-blue-600 shadow-blue-500/50 text-white border-blue-400"
            : "bg-slate-800/50 hover:bg-slate-700 text-slate-300 hover:text-white hover:shadow-blue-500/20",
        red: active
            ? "bg-red-600 shadow-red-500/50 text-white border-red-400"
            : "bg-slate-800/50 hover:bg-slate-700 text-slate-300 hover:text-white hover:shadow-red-500/20",
        green: active
            ? "bg-emerald-600 shadow-emerald-500/50 text-white border-emerald-400"
            : "bg-slate-800/50 hover:bg-slate-700 text-slate-300 hover:text-white hover:shadow-emerald-500/20",
        purple: active
            ? "bg-purple-600 shadow-purple-500/50 text-white border-purple-400"
            : "bg-slate-800/50 hover:bg-slate-700 text-slate-300 hover:text-white hover:shadow-purple-500/20",
    };

    return (
        <button
            onClick={onClick}
            className={twMerge(baseStyles, colorStyles[color] || colorStyles.blue)}
        >
            {children}
        </button>
    );
}
