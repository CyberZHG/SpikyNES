#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::pushByte(const std::uint8_t value) {
        cpuWrite(CPU_STACK_OFFSET + regs_.SP--, value);
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

    void CPU::execNOPStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            if (!opState_.isImplied) {
                (void)bus_->cpuRead(opState_.addr);
            }
            opState_.finished = true;
        }
    }

    void CPU::execLDAStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execLDXStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.X = bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execLDYStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.Y = bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execSTAStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            bus_->cpuWrite(opState_.addr, regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execSTXStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            bus_->cpuWrite(opState_.addr, regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execSTYStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            bus_->cpuWrite(opState_.addr, regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execTAXStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.X = regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execTAYStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.Y = regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execTXAStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execTYAStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A = regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execTSXStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.X = regs_.SP);
            opState_.finished = true;
        }
    }

    void CPU::execTXSStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.SP = regs_.X;
            opState_.finished = true;
        }
    }

    void CPU::execPHAStep() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            pushByte(regs_.A);
            opState_.finished = true;
        }
    }

    void CPU::execPHPStep() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
        } else if (opState_.step == 2) {
            pushByte(regs_.P | 0b00110000);
            opState_.finished = true;
        }
    }

    void CPU::execPLAStep() {
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

    void CPU::execPLPStep() {
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

    void CPU::execASLStep() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 7));
                regs_.updateNZ(regs_.A <<= 1);
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 7));
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.readDummy = static_cast<std::uint8_t>(opState_.readDummy << 1);
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readDummy);
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.finished = true;
        }
    }

    void CPU::execLSRStep() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
                regs_.updateNZ(regs_.A >>= 1);
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 0));
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.readDummy >>= 1;
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readDummy);
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
            opState_.finished = true;
        }
    }

    void CPU::execROLStep() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            const auto c = regs_.C();
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 7));
                regs_.updateNZ(regs_.A = static_cast<std::uint8_t>((regs_.A << 1) | c));
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 7));
                opState_.readLo = static_cast<std::uint8_t>((opState_.readDummy << 1) | c);
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readLo);
            bus_->cpuWrite(opState_.addr, opState_.readLo);
            opState_.finished = true;
        }
    }

    void CPU::execRORStep() {
        const auto n = opState_.addressingFinishedStep;
        if (opState_.step == n + 1) {
            const auto c = regs_.C();
            if (opState_.isImplied) {
                opState_.readDummy = bus_->cpuRead(regs_.PC);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
                regs_.updateNZ(regs_.A = static_cast<std::uint8_t>((regs_.A >> 1) | (c << 7)));
                opState_.finished = true;
            } else {
                opState_.readDummy = bus_->cpuRead(opState_.addr);
                updateBit(regs_.P, CPURegisters::INDEX_C, getBit(opState_.readDummy, 0));
                opState_.readLo = static_cast<std::uint8_t>((opState_.readDummy >> 1) | (c << 7));
            }
        } else if (opState_.step == n + 2) {
            bus_->cpuWrite(opState_.addr, opState_.readDummy);
        } else if (opState_.step == n + 3) {
            regs_.updateNZ(opState_.readLo);
            bus_->cpuWrite(opState_.addr, opState_.readLo);
            opState_.finished = true;
        }
    }

    void CPU::execANDStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A &= bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execORAStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A |= bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execEORStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.updateNZ(regs_.A ^= bus_->cpuRead(opState_.addr));
            opState_.finished = true;
        }
    }

    void CPU::execBITStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto val = bus_->cpuRead(opState_.addr);
            regs_.updateZ(val & regs_.A);
            updateBit(regs_.P, CPURegisters::INDEX_N, static_cast<std::uint8_t>(val & (1 << 7)));
            updateBit(regs_.P, CPURegisters::INDEX_V, static_cast<std::uint8_t>(val & (1 << 6)));
            opState_.finished = true;
        }
    }

    void CPU::execADCStep() {
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

    void CPU::execSBCStep() {
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

    void CPU::execCMPStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const int val = static_cast<int>(regs_.A) - static_cast<int>(m);
            updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.A);
            regs_.updateNZ(val);
            opState_.finished = true;
        }
    }

    void CPU::execCPXStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const int val = static_cast<int>(regs_.X) - static_cast<int>(m);
            updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.X);
            regs_.updateNZ(val);
            opState_.finished = true;
        }
    }

    void CPU::execCPYStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            const auto m = bus_->cpuRead(opState_.addr);
            const int val = static_cast<int>(regs_.Y) - static_cast<int>(m);
            updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.Y);
            regs_.updateNZ(val);
            opState_.finished = true;
        }
    }

    void CPU::execINCStep() {
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

    void CPU::execINXStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(++regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execINYStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(++regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execDECStep() {
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

    void CPU::execDEXStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(--regs_.X);
            opState_.finished = true;
        }
    }

    void CPU::execDEYStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC);
            regs_.updateNZ(--regs_.Y);
            opState_.finished = true;
        }
    }

    void CPU::execJMPStep() {
        if (opState_.step == opState_.addressingFinishedStep + 1) {
            regs_.PC = opState_.addr;
            opState_.finished = true;
        }
    }

    void CPU::execJSRStep() {
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

    void CPU::execRTSStep() {
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
            ++regs_.PC;
            opState_.finished = true;
        }
    }

    void CPU::execBRKStep() {
        if (opState_.step == 1) {
            opState_.readDummy = bus_->cpuRead(regs_.PC++);
        } else if (opState_.step == 2) {
            pushByte(regs_.PC >> 8);
        } else if (opState_.step == 3) {
            pushByte(regs_.PC & 0xFF);
        } else if (opState_.step == 4) {
            pushByte(regs_.P | 0b00110000);
        } else if (opState_.step == 5) {
            setBit(regs_.P, CPURegisters::INDEX_I);
            opState_.readLo = bus_->cpuRead(CPU_VECTOR_IRQ);
        } else if (opState_.step == 6) {
            opState_.readHi = bus_->cpuRead(CPU_VECTOR_IRQ + 1);
            regs_.PC = (static_cast<std::uint16_t>(opState_.readHi) << 8) | opState_.readLo;
            opState_.finished = true;
        }
    }

    void CPU::execRTIStep() {
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

    void CPU::execBCCStep() {
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

    void CPU::execBCSStep() {
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

    void CPU::execBEQStep() {
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

    void CPU::execBMIStep() {
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

    void CPU::execBNEStep() {
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

    void CPU::execBPLStep() {
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

    bool CPU::execBVC(const std::uint16_t addr) {
        if (!regs_.V()) {
            regs_.PC = addr;
            return true;
        }
        return false;
    }

    bool CPU::execBVS(const std::uint16_t addr) {
        if (regs_.V()) {
            regs_.PC = addr;
            return true;
        }
        return false;
    }

    void CPU::execCLC() {
        clearBit(regs_.P, CPURegisters::INDEX_C);
    }

    void CPU::execCLD() {
        clearBit(regs_.P, CPURegisters::INDEX_D);
    }

    void CPU::execCLI() {
        clearBit(regs_.P, CPURegisters::INDEX_I);
    }

    void CPU::execCLV() {
        clearBit(regs_.P, CPURegisters::INDEX_V);
    }

    void CPU::execSEC() {
        setBit(regs_.P, CPURegisters::INDEX_C);
    }

    void CPU::execSED() {
        setBit(regs_.P, CPURegisters::INDEX_D);
    }

    void CPU::execSEI() {
        setBit(regs_.P, CPURegisters::INDEX_I);
    }

    int CPU::handleNMI() {
        pushWord(regs_.PC);
        pushByte((regs_.P | 0b00100000) & ~(1 << CPURegisters::INDEX_B));
        setBit(regs_.P, CPURegisters::INDEX_I);
        regs_.PC = (bus_->cpuRead(CPU_VECTOR_NMI + 1) << 8) | bus_->cpuRead(CPU_VECTOR_NMI);
        nmiPending_ = false;
        return 7;
    }

    int CPU::handleIRQ() {
        pushWord(regs_.PC);
        pushByte((regs_.P | 0b00100000) & ~(1 << CPURegisters::INDEX_B));
        setBit(regs_.P, CPURegisters::INDEX_I);
        regs_.PC = (bus_->cpuRead(CPU_VECTOR_IRQ + 1) << 8) | bus_->cpuRead(CPU_VECTOR_IRQ);
        irqPending_ = false;
        return 7;
    }
}
