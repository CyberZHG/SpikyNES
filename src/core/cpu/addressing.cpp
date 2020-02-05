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

    AddressingResult CPU::addrAbsoluteIndirect() {
        AddressingResult result;
        const auto addr = read(regs_.PC + 1) << 8 | read(regs_.PC);
        result.addr = read((addr & 0xFF00) | ((addr + 1) & 0x00FF)) << 8 | read(addr);
        result.pageCrossed = false;
        regs_.PC += 2;
        return result;
    }

    AddressingResult CPU::addrZeroPage() {
        return {read(regs_.PC++), false};
    }

    AddressingResult CPU::addrZeroPageX() {
        AddressingResult result;
        result.addr = static_cast<std::uint16_t>(read(regs_.PC++));
        result.addr = (result.addr + regs_.X) & 0xFF;
        result.pageCrossed = false;
        return result;
    }

    AddressingResult CPU::addrZeroPageY() {
        AddressingResult result;
        result.addr = static_cast<std::uint16_t>(read(regs_.PC++));
        result.addr = (result.addr + regs_.Y) & 0xFF;
        result.pageCrossed = false;
        return result;
    }

    AddressingResult CPU::addrZeroPageIndirectX() {
        auto addr = static_cast<std::uint16_t>(read(regs_.PC++));
        addr = (addr + regs_.X) & 0xFF;
        AddressingResult result;
        result.addr = read((addr + 1) & 0xFF) << 8 | read(addr);
        result.pageCrossed = false;
        return result;
    }

    AddressingResult CPU::addrZeroPageIndirectY() {
        auto addr = static_cast<std::uint16_t>(read(regs_.PC++));
        addr = read((addr + 1) & 0xFF) << 8 | read(addr);
        AddressingResult result;
        result.addr = addr + regs_.Y;
        result.pageCrossed = hasPageCross(addr, result.addr);
        return result;
    }

    AddressingResult CPU::addrRelative() {
        const auto offset = static_cast<std::int8_t>(read(regs_.PC++));
        const auto addr = static_cast<std::uint16_t>(regs_.PC + offset);
        return {addr, hasPageCross(regs_.PC, addr)};
    }
}
