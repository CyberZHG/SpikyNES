#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::execLAX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = regs_.X = bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execLAS() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = regs_.X = regs_.SP = regs_.SP & bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execSAX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            bus_->cpuWrite(opState_.addr, regs_.A & regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execSHA() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto result = static_cast<std::uint8_t>(regs_.A & regs_.X & (opState_.readHi + 1));
            const auto writeAddr = opState_.hasPageCross
                ? static_cast<std::uint16_t>((result << 8) | (opState_.addr & 0xFF))
                : opState_.addr;
            bus_->cpuWrite(writeAddr, result);
            opState_.finished = true;
        }
    }

    void CPU::execSHX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto result = static_cast<std::uint8_t>(regs_.X & (opState_.readHi + 1));
            const auto writeAddr = opState_.hasPageCross
                ? static_cast<std::uint16_t>((result << 8) | (opState_.addr & 0xFF))
                : opState_.addr;
            bus_->cpuWrite(writeAddr, result);
            opState_.finished = true;
        }
    }

    void CPU::execSHY() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto result = static_cast<std::uint8_t>(regs_.Y & (opState_.readHi + 1));
            const auto writeAddr = opState_.hasPageCross
                ? static_cast<std::uint16_t>((result << 8) | (opState_.addr & 0xFF))
                : opState_.addr;
            bus_->cpuWrite(writeAddr, result);
            opState_.finished = true;
        }
    }

    void CPU::execSHS() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.SP = regs_.A & regs_.X;
            const auto result = static_cast<std::uint8_t>(regs_.SP & (opState_.readHi + 1));
            const auto writeAddr = opState_.hasPageCross
                ? static_cast<std::uint16_t>((result << 8) | (opState_.addr & 0xFF))
                : opState_.addr;
            bus_->cpuWrite(writeAddr, result);
            opState_.finished = true;
        }
    }

    void CPU::execANC() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A &= bus_->cpuRead(opState_.addr));
            updateBit(regs_.P, CPURegisters::INDEX_C, regs_.N());
            opState_.finished = true;
        }
    }

    void CPU::execARR() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.A &= bus_->cpuRead(opState_.addr);
            regs_.A = (regs_.A >> 1) | (regs_.C() << 7);
            regs_.updateNZ(regs_.A);
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 6));
            updateBit(regs_.P, CPURegisters::INDEX_V, static_cast<bool>(getBit(regs_.A, 6) ^ getBit(regs_.A, 5)));
            opState_.finished = true;
        }
    }

    void CPU::execASR() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.A &= bus_->cpuRead(opState_.addr);
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
            regs_.updateNZ(regs_.A >>= 1);
            opState_.finished = true;
        }
    }

    void CPU::execSBX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto val = regs_.A & regs_.X;
            const auto m = bus_->cpuRead(opState_.addr);
            updateBit(regs_.P, CPURegisters::INDEX_C, val >= m);
            regs_.updateNZ(regs_.X = val - m);
            opState_.finished = true;
        }
    }

    void CPU::execXAA() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = (regs_.A | 0xEE) & regs_.X & bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    // RMW: dec mem, then CMP
    void CPU::execDCP() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            --opState_.readDummy;
        } else if (opState_.step == n + 3) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            const auto result = static_cast<std::uint8_t>(regs_.A - opState_.readDummy);
            regs_.updateNZ(result);
            updateBit(regs_.P, CPURegisters::INDEX_C, regs_.A >= opState_.readDummy);
            opState_.finished = true;
        }
    }

    // RMW: inc mem, then SBC
    void CPU::execISC() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            ++opState_.readDummy;
        } else if (opState_.step == n + 3) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            const auto m = opState_.readDummy;
            const auto val = static_cast<int>(regs_.A) - static_cast<int>(m) - (1 - regs_.C());
            const auto result = static_cast<std::uint8_t>(val);
            updateBit(regs_.P, CPURegisters::INDEX_C, val >= 0);
            updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (regs_.A ^ m) & 0x80) != 0);
            regs_.updateNZ(regs_.A = result);
            opState_.finished = true;
        }
    }

    // RMW: ROL mem, then AND A
    void CPU::execRLA() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
            const auto c = regs_.C();
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 7));
            opState_.readLo = (opState_.readDummy << 1) | c;
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
        } else if (opState_.step == n + 3) {
            bus_->cpuWrite(opState_.addr, opState_.readLo);
            regs_.updateNZ(regs_.A &= opState_.readLo);
            opState_.finished = true;
        }
    }

    // RMW: ROR mem, then ADC A
    void CPU::execRRA() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
            const auto c = regs_.C();
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 0));
            opState_.readLo = (opState_.readDummy >> 1) | (c << 7);
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
        } else if (opState_.step == n + 3) {
            bus_->cpuWrite(opState_.addr, opState_.readLo);
            const auto val = static_cast<int>(regs_.A) + static_cast<int>(opState_.readLo) + regs_.C();
            const auto result = static_cast<std::uint8_t>(val);
            updateBit(regs_.P, CPURegisters::INDEX_C, val > 255);
            updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (opState_.readLo ^ result) & 0x80) != 0);
            regs_.updateNZ(regs_.A = result);
            opState_.finished = true;
        }
    }

    // RMW: ASL mem, then ORA A
    void CPU::execSLO() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 7));
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.readDummy = static_cast<std::uint8_t>(opState_.readDummy << 1);
        } else if (opState_.step == n + 3) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            regs_.updateNZ(regs_.A |= opState_.readDummy);
            opState_.finished = true;
        }
    }

    // RMW: LSR mem, then EOR A
    void CPU::execSRE() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 0));
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.readDummy = static_cast<std::uint8_t>(opState_.readDummy >> 1);
        } else if (opState_.step == n + 3) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            regs_.updateNZ(regs_.A ^= opState_.readDummy);
            opState_.finished = true;
        }
    }

    void CPU::execJAM() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            halted_ = true;
            opState_.finished = true;
        }
    }
}
