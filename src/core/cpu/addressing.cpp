#include "core/cpu.h"

namespace nes_core {
    AddressingResult CPU::addrImmediate() {
        return {regs_.PC++, false};
    }

    AddressingResult CPU::addrAbsolute() {
        AddressingResult result;
        result.address = read(regs_.PC + 1) << 8 | read(regs_.PC);
        result.pageCrossed = false;
        regs_.PC += 2;
        return result;
    }
}
