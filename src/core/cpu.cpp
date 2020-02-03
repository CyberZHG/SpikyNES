#include "core/cpu.h"

namespace nes_core {
    void CPURegisters::reset() {
        P = 0b00100100;
    }

    CPU::CPU(Bus *bus) {
        bus_ = bus;
    }

    void CPU::reset() {
        regs_.reset();
        regs_.PC = read(0xFFFD) << 8 | read(0xFFFC);
    }

    std::uint8_t CPU::read(const std::uint16_t addr) const {
        return mem_[addr % CPU_RAM_SIZE];
    }

    void CPU::write(const std::uint16_t addr, const std::uint16_t value) {
        mem_[addr % CPU_RAM_SIZE] = value;
    }

    int CPU::step() {
        return 0;
    }
}

