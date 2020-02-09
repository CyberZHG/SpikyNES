#ifndef SPIKYNES_CONSTANTS_H
#define SPIKYNES_CONSTANTS_H

#include <cstdint>
#include <cstdio>

namespace nes_core {

    constexpr std::int32_t CPU_FREQUENCY_NTSC = 1789773;  // 1.789773 MHz
    constexpr std::int32_t CPU_FREQUENCY_PAL = 1662607;  // 1.662607 MHz

    constexpr std::size_t CPU_RAM_SIZE = 0x0800;
    constexpr std::size_t CPU_STACK_OFFSET = 0x0100;
    constexpr std::size_t CPU_PROGRAM_BEGIN = 0x0200;
    constexpr std::uint16_t CPU_RAM_END = 0x2000;
    constexpr std::uint16_t CPU_VECTOR_NMI = 0xFFFA;
    constexpr std::uint16_t CPU_VECTOR_RESET = 0xFFFC;
    constexpr std::uint16_t CPU_VECTOR_IRQ = 0xFFFE;

    constexpr std::uint16_t PPU_REG_START = 0x2000;
    constexpr std::size_t PPU_REG_COUNT = 8;
    constexpr std::uint16_t PPU_REG_END = 0x4000;
    constexpr std::uint16_t PPU_MEM_MASK = 0x3FFF;
    constexpr std::uint16_t PPU_NAMETABLE_START = 0x2000;
    constexpr std::uint16_t PPU_NAMETABLE_MIRRO_START = 0x3000;
    constexpr std::size_t PPU_NAMETABLE_SIZE = 0x1000;
    constexpr std::uint16_t PPU_PALETTE_START = 0x3F00;
    constexpr std::size_t PPU_PALETTE_SIZE = 0x20;
    constexpr std::uint16_t PPU_OAM_SIZE = 0x100;

    constexpr std::uint16_t APU_REG_START = 0x4000;
    constexpr std::size_t APU_REG_COUNT = 0x18;
    constexpr std::uint16_t APU_REG_END = 0x4018;

    constexpr std::uint16_t PRG_RAM_START = 0x6000;
    constexpr std::size_t PRG_RAM_SIZE = 0x2000;
    constexpr std::size_t PRG_ROM_SIZE = 0x8000;
    constexpr std::uint16_t PRG_ROM_START = 0x8000;
    constexpr std::size_t PRG_ROM_BANK_SIZE = 0x4000;

    constexpr std::size_t CHR_ROM_BANK_SIZE = 0x2000;

    constexpr std::size_t INES_HEADER_SIZE = 16;

}

#endif //SPIKYNES_CONSTANTS_H