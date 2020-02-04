#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
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
        write(0x0100 + regs_.SP--, regs_.A);
    }

    void CPU::execPHP() {
        write(0x0100 + regs_.SP--, regs_.P);
    }

    void CPU::execPLA() {
        regs_.updateNZ(regs_.A = read(0x0100 + ++regs_.SP));
    }

    void CPU::execPLP() {
        const auto b = regs_.P & (1 << CPURegisters::INDEX_B);
        regs_.P = (read(0x0100 + ++regs_.SP) & ~(1 << CPURegisters::INDEX_B)) | b;
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
}
