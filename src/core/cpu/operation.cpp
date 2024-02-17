#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::pushByte(const std::uint8_t value) {
        bus_->cpuWrite(CPU_STACK_OFFSET + regs_.SP--, value);
    }

    void CPU::pushWord(const std::uint16_t value) {
        pushByte(value >> 8);
        pushByte(value & 0xFF);
    }

    std::uint8_t CPU::popByte() {
        return cpuRead(CPU_STACK_OFFSET + ++regs_.SP);
    }

    std::uint16_t CPU::popWord() {
        const auto low = popByte();
        const auto high = popByte();
        return (high << 8) | low;
    }

    void CPU::execNOP() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            if (!opState_.isImplied) {
                (void)bus_->cpuRead(opState_.addr);
            }
            opState_.finished = true;
        }
    }

    void CPU::execLDA() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execLDX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.X = bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execLDY() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.Y = bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execSTA() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            bus_->cpuWrite(opState_.addr, regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execSTX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            bus_->cpuWrite(opState_.addr, regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execSTY() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            bus_->cpuWrite(opState_.addr, regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execTAX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.X = regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execTAY() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.Y = regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execTXA() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execTYA() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execTSX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.X = regs_.SP);
            opState_.finished = true;
        }
    }

    void CPU::execTXS() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.SP = regs_.X;
            opState_.finished = true;
        }
    }

    void CPU::execPHA() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            pushByte(regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execPHP() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            pushByte(regs_.P | 0b00110000);
            opState_.finished = true;
        }
    }

    void CPU::execPLA() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            ++regs_.SP;
        } else if (opState_.step == 3) {
            regs_.updateNZ(regs_.A = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP));
            opState_.finished = true;
        }
    }

    void CPU::execPLP() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            ++regs_.SP;
        } else if (opState_.step == 3) {
            regs_.P = (bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP) | 0b00100000) & ~(1 << CPURegisters::INDEX_B);
            opState_.finished = true;
        }
    }

    void CPU::execASL() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 7));
                regs_.updateNZ(regs_.A <<= 1);
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 7));
            opState_.readDummy = static_cast<std::uint8_t>(opState_.readDummy << 1);
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readDummy);
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.finished = true;
        }
    }

    void CPU::execLSR() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
                regs_.updateNZ(regs_.A >>= 1);
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 0));
            opState_.readDummy >>= 1;
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readDummy);
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.finished = true;
        }
    }

    void CPU::execROL() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                const auto c = regs_.C();
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 7));
                regs_.updateNZ(regs_.A = static_cast<std::uint8_t>((regs_.A << 1) | c));
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            const auto c = regs_.C();
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 7));
            opState_.readLo = static_cast<std::uint8_t>((opState_.readDummy << 1) | c);
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readLo);
            bus_->cpuWrite(opState_.addr, opState_.readLo);
            opState_.finished = true;
        }
    }

    void CPU::execROR() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                const auto c = regs_.C();
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
                regs_.updateNZ(regs_.A = static_cast<std::uint8_t>((regs_.A >> 1) | (c << 7)));
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            const auto c = regs_.C();
            updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 0));
            opState_.readLo = static_cast<std::uint8_t>((opState_.readDummy >> 1) | (c << 7));
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readLo);
            bus_->cpuWrite(opState_.addr, opState_.readLo);
            opState_.finished = true;
        }
    }

    void CPU::execAND() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A &= bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execORA() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A |= bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execEOR() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A ^= bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execBIT() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto val = bus_->cpuRead(opState_.addr);
            regs_.updateZ(val & regs_.A);
            updateBit(regs_.P, CPURegisters::INDEX_N, static_cast<std::uint8_t>(val & (1 << 7)));
            updateBit(regs_.P, CPURegisters::INDEX_V, static_cast<std::uint8_t>(val & (1 << 6)));
            opState_.finished = true;
        }
    }

    void CPU::execADC() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const auto val = static_cast<int>(regs_.A) + static_cast<int>(m) + regs_.C();
            const auto result = static_cast<std::uint8_t>(val);
            updateBit(regs_.P, CPURegisters::INDEX_C, val > 255);
            updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (m ^ result) & 0x80) != 0);
            regs_.updateNZ(regs_.A = result);
            opState_.finished = true;
        }
    }

    void CPU::execSBC() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const auto val = static_cast<int>(regs_.A) - static_cast<int>(m) - (1 - regs_.C());
            const auto result = static_cast<std::uint8_t>(val);
            updateBit(regs_.P, CPURegisters::INDEX_C, val >= 0);
            updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (regs_.A ^ m) & 0x80) != 0);
            regs_.updateNZ(regs_.A = result);
            opState_.finished = true;
        }
    }

    void CPU::execCMP() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const int val = static_cast<int>(regs_.A) - static_cast<int>(m);
            updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.A);
            regs_.updateNZ(val);
            opState_.finished = true;
        }
    }

    void CPU::execCPX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const int val = static_cast<int>(regs_.X) - static_cast<int>(m);
            updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.X);
            regs_.updateNZ(val);
            opState_.finished = true;
        }
    }

    void CPU::execCPY() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const int val = static_cast<int>(regs_.Y) - static_cast<int>(m);
            updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.Y);
            regs_.updateNZ(val);
            opState_.finished = true;
        }
    }

    void CPU::execINC() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            ++opState_.readDummy;
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readDummy);
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.finished = true;
        }
    }

    void CPU::execINX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(++regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execINY() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(++regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execDEC() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            opState_.readDummy = bus_->cpuRead(opState_.addr);
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            --opState_.readDummy;
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readDummy);
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.finished = true;
        }
    }

    void CPU::execDEX() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(--regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execDEY() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(--regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execJMP() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.PC = opState_.addr;
            opState_.finished = true;
        }
    }

    void CPU::execJSR() {
        if (opState_.step == 1) {
            opState_.readLo = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
        } else if (opState_.step == 3) {
            pushByte(regs_.PC >> 8);
        } else if (opState_.step == 4) {
            pushByte(regs_.PC & 0xFF);
        } else if (opState_.step == 5) {
            opState_.readHi = bus_->cpuRead(regs_.PC);
            regs_.PC = (static_cast<std::uint16_t>(opState_.readHi) << 8) | opState_.readLo;
            opState_.finished = true;
        }
    }

    void CPU::execRTS() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            ++regs_.SP;
        } else if (opState_.step == 3) {
            opState_.readLo = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            ++regs_.SP;
        } else if (opState_.step == 4) {
            opState_.readHi = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            regs_.PC = (static_cast<std::uint16_t>(opState_.readHi) << 8) | opState_.readLo;
        } else if (opState_.step == 5) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            ++regs_.PC;
            opState_.finished = true;
        }
    }

    void CPU::execBRK() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            pushByte(regs_.PC >> 8);
        } else if (opState_.step == 3) {
            pushByte(regs_.PC & 0xFF);
        } else if (opState_.step == 4) {
            pushByte(regs_.P | 0b00110000);
        } else if (opState_.step == 5) {
            opState_.readLo = bus_->cpuRead(CPU_VECTOR_IRQ);
        } else if (opState_.step == 6) {
            opState_.readHi = bus_->cpuRead(CPU_VECTOR_IRQ + 1);
            regs_.PC = (static_cast<std::uint16_t>(opState_.readHi) << 8) | opState_.readLo;
            setBit(regs_.P, CPURegisters::INDEX_I);
            opState_.finished = true;
        }
    }

    void CPU::execRTI() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            ++regs_.SP;
        } else if (opState_.step == 3) {
            regs_.P = (bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP) | 0b00100000) & ~(1 << CPURegisters::INDEX_B);
            ++regs_.SP;
        } else if (opState_.step == 4) {
            opState_.readLo = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            ++regs_.SP;
        } else if (opState_.step == 5) {
            opState_.readHi = bus_->cpuRead(CPU_STACK_OFFSET + regs_.SP);
            regs_.PC = (static_cast<std::uint16_t>(opState_.readHi) << 8) | opState_.readLo;
            opState_.finished = true;
        }
    }

    void CPU::execBCC() {
        if (opState_.step == 1) {
            if (regs_.C()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execBCS() {
        if (opState_.step == 1) {
            if (!regs_.C()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execBEQ() {
        if (opState_.step == 1) {
            if (!regs_.Z()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execBMI() {
        if (opState_.step == 1) {
            if (!regs_.N()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execBNE() {
        if (opState_.step == 1) {
            if (regs_.Z()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execBPL() {
        if (opState_.step == 1) {
            if (regs_.N()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execBVC() {
        if (opState_.step == 1) {
            if (regs_.V()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execBVS() {
        if (opState_.step == 1) {
            if (!regs_.V()) {
                opState_.finished = true;
            }
        } else if (opState_.step == 2) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            if (opState_.hasPageCross) {
                opState_.baseAddr = (regs_.PC & 0xFF00) | (opState_.addr & 0x00FF);
            }
            regs_.PC = opState_.addr;
            if (!opState_.hasPageCross) {
                opState_.finished = true;
            }
        } else if (opState_.step == 3) {
            opState_.readDummy = bus_->cpuRead(opState_.baseAddr);
            opState_.finished = true;
        }
    }

    void CPU::execCLC() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            clearBit(regs_.P, CPURegisters::INDEX_C);
            opState_.finished = true;
        }
    }

    void CPU::execCLD() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            clearBit(regs_.P, CPURegisters::INDEX_D);
            opState_.finished = true;
        }
    }

    void CPU::execCLI() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            clearBit(regs_.P, CPURegisters::INDEX_I);
            opState_.finished = true;
        }
    }

    void CPU::execCLV() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            clearBit(regs_.P, CPURegisters::INDEX_V);
            opState_.finished = true;
        }
    }

    void CPU::execSEC() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            setBit(regs_.P, CPURegisters::INDEX_C);
            opState_.finished = true;
        }
    }

    void CPU::execSED() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            setBit(regs_.P, CPURegisters::INDEX_D);
            opState_.finished = true;
        }
    }

    void CPU::execSEI() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            setBit(regs_.P, CPURegisters::INDEX_I);
            opState_.finished = true;
        }
    }

    void CPU::handleNMI() {
        if (opState_.step == 0) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            pushByte(regs_.PC >> 8);
        } else if (opState_.step == 3) {
            pushByte(regs_.PC & 0xFF);
        } else if (opState_.step == 4) {
            pushByte((regs_.P | 0b00100000) & ~(1 << CPURegisters::INDEX_B));
        } else if (opState_.step == 5) {
            opState_.readLo = bus_->cpuRead(CPU_VECTOR_NMI);
        } else if (opState_.step == 6) {
            opState_.readHi = bus_->cpuRead(CPU_VECTOR_NMI + 1);
            regs_.PC = (static_cast<std::uint16_t>(opState_.readHi) << 8) | opState_.readLo;
            setBit(regs_.P, CPURegisters::INDEX_I);
            nmiActive_ = false;
            opState_.finished = true;
        }
    }

    void CPU::handleIRQ() {
        if (opState_.step == 0) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            pushByte(regs_.PC >> 8);
        } else if (opState_.step == 3) {
            pushByte(regs_.PC & 0xFF);
        } else if (opState_.step == 4) {
            pushByte((regs_.P | 0b00100000) & ~(1 << CPURegisters::INDEX_B));
            if (nmiPending_) {
                nmiPending_ = false;
                nmiActive_ = true;
                irqActive_ = false;
            }
        } else if (opState_.step == 5) {
            opState_.readLo = bus_->cpuRead(CPU_VECTOR_IRQ);
        } else if (opState_.step == 6) {
            opState_.readHi = bus_->cpuRead(CPU_VECTOR_IRQ + 1);
            regs_.PC = (static_cast<std::uint16_t>(opState_.readHi) << 8) | opState_.readLo;
            setBit(regs_.P, CPURegisters::INDEX_I);
            irqActive_ = false;
            opState_.finished = true;
        }
    }
}
