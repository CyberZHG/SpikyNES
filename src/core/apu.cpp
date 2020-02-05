#include "core/apu.h"

namespace nes_core {
    APU::APU(Bus *bus) {
        bus_ = bus;
    }

    void APU::reset() {
    }

    std::uint8_t APU::read(const std::uint16_t addr) const {
        return regs_[addr - APU_REG_START];
    }

    void APU::write(const std::uint16_t addr, const std::uint8_t value) {
        regs_[addr - APU_REG_START] = value;
    }
}
