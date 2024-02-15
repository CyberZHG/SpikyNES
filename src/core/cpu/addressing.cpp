#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    static bool hasPageCross(const std::uint16_t a, const std::uint16_t b) {
        return (a & 0xFF00) != (b & 0xFF00);
    }

    AddressingResult CPU::addrImmediate() {
        return {regs_.PC++, false};
    }

    void CPU::addrImmediateStep() {
        opState_.addr = regs_.PC++;
        opState_.addressingFinishedStep = 0;
    }

    AddressingResult CPU::addrAbsolute() {
        AddressingResult result;
        result.addr = bus_->cpuRead(regs_.PC + 1) << 8 | bus_->cpuRead(regs_.PC);
        result.pageCrossed = false;
        regs_.PC += 2;
        return result;
    }

    void CPU::addrAbsoluteStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        }
        if (opState_.step == 2) {
            opState_.readHi = bus_->cpuRead(regs_.PC++);
            opState_.addr = (opState_.readHi << 8) | opState_.readLo;
            opState_.addressingFinishedStep = 2;
        }
    }

    AddressingResult CPU::addrAbsoluteX() {
        AddressingResult result;
        const auto addr = bus_->cpuRead(regs_.PC + 1) << 8 | bus_->cpuRead(regs_.PC);
        result.addr = addr + regs_.X;
        result.pageCrossed = hasPageCross(addr, result.addr);
        regs_.PC += 2;
        return result;
    }

    void CPU::addrAbsoluteXStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readHi = bus_->cpuRead(regs_.PC++);
            opState_.baseAddr = (opState_.readHi << 8) | opState_.readLo;
            opState_.addr = opState_.baseAddr + regs_.X;
            opState_.hasPageCross = hasPageCross(opState_.baseAddr, opState_.addr);
            if (!opState_.hasPageCross) {
                opState_.addressingFinishedStep = 2;
            }
        } else if (opState_.step == 3 && opState_.hasPageCross) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.addressingFinishedStep = 3;
        }
    }

    void CPU::addrAbsoluteYStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readHi = bus_->cpuRead(regs_.PC++);
            opState_.baseAddr = (opState_.readHi << 8) | opState_.readLo;
            opState_.addr = opState_.baseAddr + regs_.Y;
            opState_.hasPageCross = hasPageCross(opState_.baseAddr, opState_.addr);
            if (!opState_.hasPageCross) {
                opState_.addressingFinishedStep = 2;
            }
        } else if (opState_.step == 3 && opState_.hasPageCross) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.addressingFinishedStep = 3;
        }
    }

    AddressingResult CPU::addrAbsoluteY() {
        AddressingResult result;
        const auto addr = bus_->cpuRead(regs_.PC + 1) << 8 | bus_->cpuRead(regs_.PC);
        result.addr = addr + regs_.Y;
        result.pageCrossed = hasPageCross(addr, result.addr);
        regs_.PC += 2;
        return result;
    }

    AddressingResult CPU::addrAbsoluteIndirect() {
        AddressingResult result;
        const auto addr = bus_->cpuRead(regs_.PC + 1) << 8 | bus_->cpuRead(regs_.PC);
        result.addr = bus_->cpuRead((addr & 0xFF00) | ((addr + 1) & 0x00FF)) << 8 | bus_->cpuRead(addr);
        result.pageCrossed = false;
        regs_.PC += 2;
        return result;
    }

    void CPU::addrZeroPageStep() {
        if (opState_.step == 1) {
            opState_.addr = bus_->cpuRead(regs_.PC++);
            opState_.addressingFinishedStep = 1;
        }
    }

    AddressingResult CPU::addrZeroPage() {
        return {bus_->cpuRead(regs_.PC++), false};
    }

    void CPU::addrZeroPageXStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(opState_.readLo);
            opState_.addr = (opState_.readLo + regs_.X) & 0xFF;
            opState_.addressingFinishedStep = 2;
        }
    }

    AddressingResult CPU::addrZeroPageX() {
        AddressingResult result;
        result.addr = static_cast<std::uint16_t>(bus_->cpuRead(regs_.PC++));
        result.addr = (result.addr + regs_.X) & 0xFF;
        result.pageCrossed = false;
        return result;
    }

    void CPU::addrZeroPageYStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(opState_.readLo);
            opState_.addr = (opState_.readLo + regs_.Y) & 0xFF;
            opState_.addressingFinishedStep = 2;
        }
    }

    AddressingResult CPU::addrZeroPageY() {
        AddressingResult result;
        result.addr = static_cast<std::uint16_t>(bus_->cpuRead(regs_.PC++));
        result.addr = (result.addr + regs_.Y) & 0xFF;
        result.pageCrossed = false;
        return result;
    }

    AddressingResult CPU::addrZeroPageIndirectX() {
        auto addr = static_cast<std::uint16_t>(bus_->cpuRead(regs_.PC++));
        addr = (addr + regs_.X) & 0xFF;
        AddressingResult result;
        result.addr = bus_->cpuRead((addr + 1) & 0xFF) << 8 | bus_->cpuRead(addr);
        result.pageCrossed = false;
        return result;
    }

    void CPU::addrZeroPageIndirectXStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(opState_.readLo);
            opState_.baseAddr = (opState_.readLo + regs_.X) & 0xFF;
        } else if (opState_.step == 3) {
            opState_.readLo = bus_->cpuRead(opState_.baseAddr);
        } else if (opState_.step == 4) {
            opState_.readHi = bus_->cpuRead((opState_.baseAddr + 1) & 0xFF);
            opState_.addr = (opState_.readHi << 8) | opState_.readLo;
            opState_.addressingFinishedStep = 4;
        }
    }

    AddressingResult CPU::addrZeroPageIndirectY() {
        auto addr = static_cast<std::uint16_t>(bus_->cpuRead(regs_.PC++));
        addr = bus_->cpuRead((addr + 1) & 0xFF) << 8 | bus_->cpuRead(addr);
        AddressingResult result;
        result.addr = addr + regs_.Y;
        result.pageCrossed = hasPageCross(addr, result.addr);
        return result;
    }

    void CPU::addrZeroPageIndirectYStep() {
        if (opState_.step == 1) {
            opState_.baseAddr = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readLo = bus_->cpuRead(opState_.baseAddr);
        } else if (opState_.step == 3) {
            opState_.readHi = bus_->cpuRead((opState_.baseAddr + 1) & 0xFF);
            opState_.baseAddr = (opState_.readHi << 8) | opState_.readLo;
            opState_.addr = opState_.baseAddr + regs_.Y;
            opState_.hasPageCross = hasPageCross(opState_.baseAddr, opState_.addr);
            if (!opState_.hasPageCross) {
                opState_.addressingFinishedStep = 3;
            }
        } else if (opState_.step == 4 && opState_.hasPageCross) {
            opState_.readDummy = bus_->cpuRead((opState_.readHi << 8) | static_cast<std::uint8_t>(opState_.readLo + regs_.Y));
            opState_.addressingFinishedStep = 4;
        }
    }

    AddressingResult CPU::addrRelative() {
        const auto offset = static_cast<std::int8_t>(bus_->cpuRead(regs_.PC++));
        const auto addr = static_cast<std::uint16_t>(regs_.PC + offset);
        return {addr, hasPageCross(regs_.PC, addr)};
    }
}
