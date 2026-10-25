# spiky-nes

JavaScript/WebAssembly bindings for [SpikyNES](https://github.com/CyberZHG/SpikyNES), a toy NES emulator.

Supported mappers: 0 (NROM), 1 (MMC1), 2 (UxROM), 3 (CNROM), 4 (MMC3).

## Installation

```bash
npm install spiky-nes
```

## Quick Start

```javascript
import { SCREEN_WIDTH, SCREEN_HEIGHT, Button, Emulator } from 'spiky-nes';

const emulator = new Emulator();
const rom = new Uint8Array(await (await fetch('game.nes')).arrayBuffer());
if (!emulator.loadROM(rom)) {
    throw new Error('Unsupported ROM');
}

const context = canvas.getContext('2d');
function frame() {
    const audio = emulator.runFrame(44100);  // Float32Array
    context.putImageData(new ImageData(emulator.frameBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT), 0, 0);
    requestAnimationFrame(frame);
}
requestAnimationFrame(frame);

window.addEventListener('keydown', (e) => e.key === 'Enter' && emulator.press(Button.START));
window.addEventListener('keyup', (e) => e.key === 'Enter' && emulator.release(Button.START));
```
