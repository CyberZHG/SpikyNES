#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::execLDA(const std::uint16_t addr) {
        regs_.A = bus_->read(addr);
        regs_.updateNZ(regs_.A);
    }
}
