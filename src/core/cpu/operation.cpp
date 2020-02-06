#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::pushByte(const std::uint8_t value) {
        write(CPU_STACK_OFFSET + regs_.SP--, value);
    }

    void CPU::pushWord(const std::uint16_t value) {
        pushByte(value >> 8);
        pushByte(value & 0xFF);
    }

    std::uint8_t CPU::popByte() {
        return read(CPU_STACK_OFFSET + ++regs_.SP);
    }

    std::uint16_t CPU::popWord() {
        const auto low = popByte();
        const auto high = popByte();
        return (high << 8) | low;
    }

    void CPU::execLDA(const std::uint16_t addr) {
        regs_.updateNZ(regs_.A = bus_->read(addr));
    }

    void CPU::execLDX(const std::uint16_t addr) {
        regs_.updateNZ(regs_.X = bus_->read(addr));
    }

    void CPU::execLDY(const std::uint16_t addr) {
        regs_.updateNZ(regs_.Y = bus_->read(addr));
    }

    void CPU::execSTA(const std::uint16_t addr) const {
        bus_->write(addr, regs_.A);
    }

    void CPU::execSTX(const std::uint16_t addr) const {
        bus_->write(addr, regs_.X);
    }

    void CPU::execSTY(const std::uint16_t addr) const {
        bus_->write(addr, regs_.Y);
    }

    void CPU::execTAX() {
        regs_.updateNZ(regs_.X = regs_.A);
    }

    void CPU::execTAY() {
        regs_.updateNZ(regs_.Y = regs_.A);
    }

    void CPU::execTXA() {
        regs_.updateNZ(regs_.A = regs_.X);
    }

    void CPU::execTYA() {
        regs_.updateNZ(regs_.A = regs_.Y);
    }

    void CPU::execTSX() {
        regs_.updateNZ(regs_.X = regs_.SP);
    }

    void CPU::execTXS() {
        regs_.SP = regs_.X;
    }

    void CPU::execPHA() {
        pushByte(regs_.A);
    }

    void CPU::execPHP() {
        pushByte(regs_.P | 0b00110000);
    }

    void CPU::execPLA() {
        regs_.updateNZ(regs_.A = popByte());
    }

    void CPU::execPLP() {
        regs_.P = (popByte() | 0b00100000) & ~(1 << CPURegisters::INDEX_B);
    }

    void CPU::execASL() {
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 7));
        regs_.updateNZ(regs_.A <<= 1);
    }

    void CPU::execASL(const std::uint16_t addr) {
        auto val = bus_->read(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 7));
        regs_.updateNZ(val <<= 1);
        bus_->write(addr, val);
    }

    void CPU::execLSR() {
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
        regs_.updateNZ(regs_.A >>= 1);
    }

    void CPU::execLSR(const std::uint16_t addr) {
        auto val = bus_->read(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 0));
        regs_.updateNZ(val >>= 1);
        bus_->write(addr, val);
    }

    void CPU::execROL() {
        const auto c = regs_.C();
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 7));
        regs_.updateNZ(regs_.A = (regs_.A << 1) | c);
    }

    void CPU::execROL(const std::uint16_t addr) {
        const auto c = regs_.C();
        auto val = bus_->read(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 7));
        regs_.updateNZ(val = (val << 1) | c);
        bus_->write(addr, val);
    }

    void CPU::execROR() {
        const auto c = regs_.C();
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
        regs_.updateNZ(regs_.A = (regs_.A >> 1) | (c << 7));
    }

    void CPU::execROR(const std::uint16_t addr) {
        const auto c = regs_.C();
        auto val = bus_->read(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 0));
        regs_.updateNZ(val = (val >> 1) | (c << 7));
        bus_->write(addr, val);
    }

    void CPU::execAND(const std::uint16_t addr) {
        const auto val = bus_->read(addr);
        regs_.updateNZ(regs_.A &= val);
    }

    void CPU::execORA(const std::uint16_t addr) {
        const auto val = bus_->read(addr);
        regs_.updateNZ(regs_.A |= val);
    }

    void CPU::execEOR(const std::uint16_t addr) {
        const auto val = bus_->read(addr);
        regs_.updateNZ(regs_.A ^= val);
    }

    void CPU::execBIT(const std::uint16_t addr) {
        const auto val = bus_->read(addr);
        regs_.updateZ(val & regs_.A);
        updateBit(regs_.P, CPURegisters::INDEX_N, static_cast<std::uint8_t>(val & (1 << 7)));
        updateBit(regs_.P, CPURegisters::INDEX_V, static_cast<std::uint8_t>(val & (1 << 6)));
    }

    void CPU::execADC(const std::uint16_t addr) {
        const auto m = bus_->read(addr);
        const auto val = static_cast<int>(regs_.A) + static_cast<int>(m) + regs_.C();
        const auto result = static_cast<std::uint8_t>(val);
        updateBit(regs_.P, CPURegisters::INDEX_C, val > 255);
        updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (m ^ result) & 0x80) != 0);
        regs_.updateNZ(regs_.A = result);
    }

    void CPU::execSBC(const std::uint16_t addr) {
        const auto m = bus_->read(addr);
        const auto val = static_cast<int>(regs_.A) - static_cast<int>(m) - (1 - regs_.C());
        const auto result = static_cast<std::uint8_t>(val);
        updateBit(regs_.P, CPURegisters::INDEX_C, val >= 0);
        updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (regs_.A ^ m) & 0x80) != 0);
        regs_.updateNZ(regs_.A = result);
    }

    void CPU::execCMP(const std::uint16_t addr) {
        const auto m = bus_->read(addr);
        const int val = static_cast<int>(regs_.A) - static_cast<int>(m);
        updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.A);
        regs_.updateNZ(val);
    }

    void CPU::execCPX(const std::uint16_t addr) {
        const auto m = bus_->read(addr);
        const int val = static_cast<int>(regs_.X) - static_cast<int>(m);
        updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.X);
        regs_.updateNZ(val);
    }

    void CPU::execCPY(const std::uint16_t addr) {
        const auto m = bus_->read(addr);
        const int val = static_cast<int>(regs_.Y) - static_cast<int>(m);
        updateBit(regs_.P, CPURegisters::INDEX_C, m <= regs_.Y);
        regs_.updateNZ(val);
    }

    void CPU::execINC(const std::uint16_t addr) {
        const auto val = bus_->read(addr) + 1;
        bus_->write(addr, val);
        regs_.updateNZ(val);
    }

    void CPU::execINX() {
        regs_.updateNZ(++regs_.X);
    }

    void CPU::execINY() {
        regs_.updateNZ(++regs_.Y);
    }

    void CPU::execDEC(const std::uint16_t addr) {
        const auto val = bus_->read(addr) - 1;
        bus_->write(addr, val);
        regs_.updateNZ(val);
    }

    void CPU::execDEX() {
        regs_.updateNZ(--regs_.X);
    }

    void CPU::execDEY() {
        regs_.updateNZ(--regs_.Y);
    }

    void CPU::execJMP(const std::uint16_t addr) {
        regs_.PC = addr;
    }

    void CPU::execJSR(const std::uint16_t addr) {
        pushWord(regs_.PC - 1);
        regs_.PC = addr;
    }

    void CPU::execRTS() {
        regs_.PC = popWord() + 1;
    }

    void CPU::execBRK() {
        pushWord(regs_.PC + 1);
        pushByte(regs_.P | 0b00110000);
        setBit(regs_.P, CPURegisters::INDEX_I);
        regs_.PC = (bus_->read(CPU_VECTOR_IRQ + 1) << 8) | bus_->read(CPU_VECTOR_IRQ);
    }

    void CPU::execRTI() {
        regs_.P = (popByte() | 0b00100000) & ~(1 << CPURegisters::INDEX_B);
        regs_.PC = popWord();
    }

    bool CPU::execBCC(const std::uint16_t addr) {
        if (!regs_.C()) {
            regs_.PC = addr;
            return true;
        }
        return false;
    }

    bool CPU::execBCS(const std::uint16_t addr) {
        if (regs_.C()) {
            regs_.PC = addr;
            return true;
        }
        return false;
    }

    bool CPU::execBEQ(const std::uint16_t addr) {
        if (regs_.Z()) {
            regs_.PC = addr;
            return true;
        }
        return false;
    }

    bool CPU::execBMI(const std::uint16_t addr) {
        if (regs_.N()) {
            regs_.PC = addr;
            return true;
        }
        return false;
    }

    bool CPU::execBNE(const std::uint16_t addr) {
        if (!regs_.Z()) {
            regs_.PC = addr;
            return true;
        }
        return false;
    }

    bool CPU::execBPL(const std::uint16_t addr) {
        if (!regs_.N()) {
            regs_.PC = addr;
            return true;
        }
        return false;
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
        regs_.PC = (bus_->read(CPU_VECTOR_NMI + 1) << 8) | bus_->read(CPU_VECTOR_NMI);
        nmiPending_ = false;
        return 7;
    }

    int CPU::handleIRQ() {
        pushWord(regs_.PC);
        pushByte((regs_.P | 0b00100000) & ~(1 << CPURegisters::INDEX_B));
        setBit(regs_.P, CPURegisters::INDEX_I);
        regs_.PC = (bus_->read(CPU_VECTOR_IRQ + 1) << 8) | bus_->read(CPU_VECTOR_IRQ);
        irqPending_ = false;
        return 7;
    }
}
