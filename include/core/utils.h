#ifndef SPIKYNES_UTILS_H
#define SPIKYNES_UTILS_H

#include <cstdint>

namespace nes_core {
    inline bool getBit(const std::uint8_t& byte, const int bit) { return byte & (1 << bit); }
    inline void setBit(std::uint8_t& byte, const int bit) { byte |= 1 << bit; }
    inline void clearBit(std::uint8_t& byte, const int bit) { byte &= ~(1 << bit); }
}

#endif //SPIKYNES_UTILS_H