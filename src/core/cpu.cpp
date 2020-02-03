#include "core/cpu.h"

namespace nes_core {
    static const std::array<int, 256> OPCODE_BASE_CYCLES = {{
        7,6,2,8,3,3,5,5,3,2,2,2,4,4,6,6, // 0x00-0x0F
        2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7, // 0x10-0x1F
        6,6,2,8,3,3,5,5,4,2,2,2,4,4,6,6, // 0x20-0x2F
        2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7, // 0x30-0x3F
        6,6,2,8,3,3,5,5,3,2,2,2,3,4,6,6, // 0x40-0x4F
        2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7, // 0x50-0x5F
        6,6,2,8,3,3,5,5,4,2,2,2,5,4,6,6, // 0x60-0x6F
        2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7, // 0x70-0x7F
        2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4, // 0x80-0x8F
        2,6,2,6,4,4,4,4,2,5,2,5,5,5,5,5, // 0x90-0x9F
        2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4, // 0xA0-0xAF
        2,5,2,5,4,4,4,4,2,4,2,4,4,4,4,4, // 0xB0-0xBF
        2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6, // 0xC0-0xCF
        2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7, // 0xD0-0xDF
        2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6, // 0xE0-0xEF
        2,5,2,8,4,4,6,6,2,4,2,7,4,4,7,7  // 0xF0-0xFF
    }};

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
        const auto opcode = read(regs_.PC++);
        int cycles = OPCODE_BASE_CYCLES[opcode];
        switch (opcode) {
            case OPCODE_NOP: break;
            default:
                throw std::runtime_error("Invalid opcode");
        }
        return cycles;
    }
}

