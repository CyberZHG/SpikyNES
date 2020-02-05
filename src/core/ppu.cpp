#include "core/ppu.h"

namespace nes_core {
    PPU::PPU(Bus *bus) {
        bus_ = bus;
    }

    void PPU::reset() {
    }

    std::uint8_t PPU::read(const std::uint16_t addr) const {
        return regs_[(addr - PPU_REG_START) % PPU_REG_COUNT];
    }

    void PPU::write(const std::uint16_t addr, const std::uint8_t value) {
        regs_[(addr - PPU_REG_START) % PPU_REG_COUNT] = value;
    }
}
