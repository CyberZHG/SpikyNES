SpikyNES
========

A toy NES emulator.

Supported mappers: 0 (NROM), 1 (MMC1), 2 (UxROM), 3 (CNROM), 4 (MMC3).

## Build

```bash
cmake -B build -DSPIKY_NES_ENABLE_UI=ON
cmake --build build
./build/SpikyNesUI
```

## Reference

- [Nes Dev Wiki](https://www.nesdev.org/wiki/)
- [6502 Family CPU Reference](https://www.pagetable.com/c64ref/6502/)
