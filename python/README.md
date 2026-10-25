# spiky-nes

Python bindings for [SpikyNES](https://github.com/CyberZHG/SpikyNES), a toy NES emulator.

Supported mappers: 0 (NROM), 1 (MMC1), 2 (UxROM), 3 (CNROM), 4 (MMC3).

## Installation

```bash
pip install spiky-nes
```

## Quick Start

```python
import spiky_nes

emulator = spiky_nes.Emulator()
assert emulator.load_rom("game.nes")

emulator.press(spiky_nes.Button.START)
audio = emulator.run_frame(sample_rate=44100)
emulator.release(spiky_nes.Button.START)

rgba = emulator.frame_buffer()  # 256 * 240 * 4 bytes
```

Saving a frame with Pillow:

```python
from PIL import Image

Image.frombytes("RGBA", (spiky_nes.SCREEN_WIDTH, spiky_nes.SCREEN_HEIGHT), emulator.frame_buffer()).save("frame.png")
```

