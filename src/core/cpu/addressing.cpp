#include "core/cpu.h"

namespace nes_core {
    static bool hasPageCross(const std::uint16_t a, const std::uint16_t b) {
        return (a & 0xFF00) != (b & 0xFF00);
    }

    AddressingResult CPU::addrImmediate() {
        return {regs_.PC++, false};
    }

    AddressingResult CPU::addrAbsolute() {
        AddressingResult result;
        result.addr = read(regs_.PC + 1) << 8 | read(regs_.PC);
        result.pageCrossed = false;
        regs_.PC += 2;
        return result;
    }

    AddressingResult CPU::addrAbsoluteX() {
        AddressingResult result;
        const auto addr = read(regs_.PC + 1) << 8 | read(regs_.PC);
        result.addr = addr + regs_.X;
        result.pageCrossed = hasPageCross(addr, result.addr);
        regs_.PC += 2;
        return result;
    }

    AddressingResult CPU::addrAbsoluteY() {
        AddressingResult result;
        const auto addr = read(regs_.PC + 1) << 8 | read(regs_.PC);
        result.addr = addr + regs_.Y;
        result.pageCrossed = hasPageCross(addr, result.addr);
        regs_.PC += 2;
        return result;
    }

    AddressingResult CPU::addrZeroPage() {
        return {read(regs_.PC++), false};
    }
}
