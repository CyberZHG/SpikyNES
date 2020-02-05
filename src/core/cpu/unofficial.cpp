#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::execLAX(const std::uint16_t addr) {
        regs_.updateNZ(regs_.A = regs_.X = bus_->read(addr));
    }

    void CPU::execLAS(const std::uint16_t addr) {
        regs_.updateNZ(regs_.A = regs_.X = regs_.SP = regs_.SP & bus_->read(addr));
    }

    void CPU::execSAX(const std::uint16_t addr) {
        bus_->write(addr, regs_.A & regs_.X);
    }

    void CPU::execSHA(const std::uint16_t addr) {
        bus_->write(addr, regs_.A & regs_.X & ((addr >> 8) + 1));
    }

    void CPU::execSHX(const std::uint16_t addr) {
        bus_->write(addr, regs_.X & ((addr >> 8) + 1));
    }

    void CPU::execSHY(const std::uint16_t addr) {
        bus_->write(addr, regs_.Y & ((addr >> 8) + 1));
    }

    void CPU::execSHS(const std::uint16_t addr) {
        regs_.SP = regs_.A & regs_.X;
        bus_->write(addr, regs_.SP & ((addr >> 8) + 1));
    }
}
