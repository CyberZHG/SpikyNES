import SpikyNesWASMModule from './wasm/SpikyNesWASM.js';

const SpikyNesWASM = await SpikyNesWASMModule();

const SCREEN_WIDTH = SpikyNesWASM.SCREEN_WIDTH;
const SCREEN_HEIGHT = SpikyNesWASM.SCREEN_HEIGHT;
const Button = SpikyNesWASM.Button;
const CPURegisters = SpikyNesWASM.CPURegisters;
const Emulator = SpikyNesWASM.Emulator;

Emulator.prototype.runFrame = function(sampleRate = 44100) {
    return this._runFrame(sampleRate);
};
Emulator.prototype.press = function(button, player = 1) {
    this._press(button, player);
};
Emulator.prototype.release = function(button, player = 1) {
    this._release(button, player);
};

export {
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    Button,
    CPURegisters,
    Emulator,
};
