#include "core/cartridge.h"

namespace nes_core {
    std::uint8_t Cartridge::read(const std::uint16_t addr) const {
        return prg_[addr - PRG_ROM_START];
    }

    void Cartridge::write(const std::uint16_t addr, const std::uint8_t value) {
        prg_[addr - PRG_ROM_START] = value;
    }
}