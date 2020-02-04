#include "core/cpu.h"

namespace nes_core {
    AddressingResult CPU::addrImmediate() {
        return {regs_.PC++, false};
    }
}