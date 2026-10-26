SpikyNES
========

A toy NES emulator.

## Supported Mappers

| ID |     Name     |                                      Screenshot                                      | ID  | Name  |                                      Screenshot                                      |
|:--:|:------------:|:------------------------------------------------------------------------------------:|:---:|:-----:|:------------------------------------------------------------------------------------:|
| 0  |     NROM     | ![](https://github.com/user-attachments/assets/63170c28-883c-48d6-827e-87b49db437c5) |  1  | MMC1  | ![](https://github.com/user-attachments/assets/ccb4ddde-9d73-4f12-b29c-6b3b414fa06d) |
| 2  |    UxROM     | ![](https://github.com/user-attachments/assets/2f4fcfb2-3329-44e3-9857-db3a39879625) |  3  | CNROM | ![](https://github.com/user-attachments/assets/d37bb1dd-ef5e-47e6-ae39-c1a33b4791c6) |
| 4  |     MMC3     | ![](https://github.com/user-attachments/assets/f243b95f-f529-4e86-b120-5d04fcf54e39) |  7  | AxROM | ![](https://github.com/user-attachments/assets/14e57a43-5374-48f0-be1b-baffc1c48b31) |
| 11 | Color Dreams | ![](https://github.com/user-attachments/assets/ac50add6-b6dd-4608-97ee-ace96f07e9ce) | 206 | DxROM | ![](https://github.com/user-attachments/assets/96b7e8fe-d5a2-4421-a019-be95665c4c82) |

## Build

```bash
cmake -B build -DSPIKY_NES_ENABLE_UI=ON
cmake --build build
./build/SpikyNesUI
```

## Reference

- [Nes Dev Wiki](https://www.nesdev.org/wiki/)
- [6502 Family CPU Reference](https://www.pagetable.com/c64ref/6502/)
