#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    static bool hasPageCross(const std::uint16_t a, const std::uint16_t b) {
        return (a & 0xFF00) != (b & 0xFF00);
    }

    void CPU::addrImmediateStep() {
        opState_.addr = regs_.PC++;
        opState_.addressingFinishedStep = 0;
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

    void CPU::addrAbsoluteXStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readHi = bus_->cpuRead(regs_.PC++);
            opState_.baseAddr = (opState_.readHi << 8) | opState_.readLo;
            opState_.addr = opState_.baseAddr + regs_.X;
            opState_.hasPageCross = hasPageCross(opState_.baseAddr, opState_.addr);
            if (!opState_.hasPageCross && !opState_.isStore) {
                opState_.addressingFinishedStep = 2;
            }
        } else if (opState_.step == 3 && (opState_.hasPageCross || opState_.isStore)) {
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
            if (!opState_.hasPageCross && !opState_.isStore) {
                opState_.addressingFinishedStep = 2;
            }
        } else if (opState_.step == 3 && (opState_.hasPageCross || opState_.isStore)) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.addressingFinishedStep = 3;
        }
    }

    void CPU::addrAbsoluteIndirectStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readHi = bus_->cpuRead(regs_.PC++);
            opState_.baseAddr = (opState_.readHi << 8) | opState_.readLo;
        } else if (opState_.step == 3) {
            opState_.readLo = bus_->cpuRead(opState_.baseAddr);
        } else if (opState_.step == 4) {
            opState_.readHi = bus_->cpuRead((opState_.baseAddr & 0xFF00) | ((opState_.baseAddr + 1) & 0xFF));
            opState_.addr = (opState_.readHi << 8) | opState_.readLo;
            opState_.addressingFinishedStep = 3;
        }
    }

    void CPU::addrZeroPageStep() {
        if (opState_.step == 1) {
            opState_.addr = bus_->cpuRead(regs_.PC++);
            opState_.addressingFinishedStep = 1;
        }
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

    void CPU::addrZeroPageYStep() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(opState_.readLo);
            opState_.addr = (opState_.readLo + regs_.Y) & 0xFF;
            opState_.addressingFinishedStep = 2;
        }
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
            if (!opState_.hasPageCross && !opState_.isStore) {
                opState_.addressingFinishedStep = 3;
            }
        } else if (opState_.step == 4 && (opState_.hasPageCross || opState_.isStore)) {
            opState_.readDummy = bus_->cpuRead((opState_.readHi << 8) | static_cast<std::uint8_t>(opState_.readLo + regs_.Y));
            opState_.addressingFinishedStep = 4;
        }
    }

    void CPU::addrRelativeStep() {
        if (opState_.step == 1) {
            const auto offset = static_cast<std::int8_t>(bus_->cpuRead(regs_.PC++));
            opState_.addr = regs_.PC + offset;
            opState_.hasPageCross = hasPageCross(regs_.PC, opState_.addr);
            opState_.addressingFinishedStep = 0;
        }
    }
}
