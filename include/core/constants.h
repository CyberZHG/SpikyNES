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
    constexpr std::size_t CPU_RAM_END = 0x2000;

    constexpr std::size_t PPU_RAM_END = 0x4000;

}

#endif //SPIKYNES_CONSTANTS_H