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

    void CPU::execSTA(const std::uint16_t addr) const {
        bus_->write(addr, regs_.A);
    }

    void CPU::execSTX(const std::uint16_t addr) const {
        bus_->write(addr, regs_.X);
    }

    void CPU::execSTY(const std::uint16_t addr) const {
        bus_->write(addr, regs_.Y);
    }

    void CPU::execTAX() {
        regs_.updateNZ(regs_.X = regs_.A);
    }

    void CPU::execTAY() {
        regs_.updateNZ(regs_.Y = regs_.A);
    }

    void CPU::execTXA() {
        regs_.updateNZ(regs_.A = regs_.X);
    }

    void CPU::execTYA() {
        regs_.updateNZ(regs_.A = regs_.Y);
    }

    void CPU::execTSX() {
        regs_.updateNZ(regs_.X = regs_.SP);
    }

    void CPU::execTXS() {
        regs_.SP = regs_.X;
    }
}
