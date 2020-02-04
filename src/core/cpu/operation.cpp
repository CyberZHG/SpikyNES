#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::execLDA(const std::uint16_t addr) {
        regs_.A = bus_->read(addr);
        regs_.updateNZ(regs_.A);
    }

    void CPU::execLDX(const std::uint16_t addr) {
        regs_.X = bus_->read(addr);
        regs_.updateNZ(regs_.X);
    }

    void CPU::execLDY(const std::uint16_t addr) {
        regs_.Y = bus_->read(addr);
        regs_.updateNZ(regs_.Y);
    }
}
