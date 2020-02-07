#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    void CPU::execLAX(const std::uint16_t addr) {
        regs_.updateNZ(regs_.A = regs_.X = bus_->cpuRead(addr));
    }

    void CPU::execLAS(const std::uint16_t addr) {
        regs_.updateNZ(regs_.A = regs_.X = regs_.SP = regs_.SP & bus_->cpuRead(addr));
    }

    void CPU::execSAX(const std::uint16_t addr) const {
        bus_->cpuWrite(addr, regs_.A & regs_.X);
    }

    void CPU::execSHA(const std::uint16_t addr) const {
        bus_->cpuWrite(addr, regs_.A & regs_.X & ((addr >> 8) + 1));
    }

    void CPU::execSHX(const std::uint16_t addr) const {
        bus_->cpuWrite(addr, regs_.X & ((addr >> 8) + 1));
    }

    void CPU::execSHY(const std::uint16_t addr) const {
        bus_->cpuWrite(addr, regs_.Y & ((addr >> 8) + 1));
    }

    void CPU::execSHS(const std::uint16_t addr) {
        regs_.SP = regs_.A & regs_.X;
        bus_->cpuWrite(addr, regs_.SP & ((addr >> 8) + 1));
    }

    void CPU::execANC(const std::uint16_t addr) {
        regs_.updateNZ(regs_.A &= bus_->cpuRead(addr));
        updateBit(regs_.P, CPURegisters::INDEX_C, regs_.N());
    }

    void CPU::execARR(const std::uint16_t addr) {
        regs_.A &= bus_->cpuRead(addr);
        regs_.A = (regs_.A >> 1) | (regs_.C() << 7);
        regs_.updateNZ(regs_.A);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 6));
        updateBit(regs_.P, CPURegisters::INDEX_V, static_cast<bool>(getBit(regs_.A, 6) ^ getBit(regs_.A, 5)));
    }

    void CPU::execASR(const std::uint16_t addr) {
        regs_.A &= bus_->cpuRead(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(regs_.A, 0));
        regs_.updateNZ(regs_.A >>= 1);
    }

    void CPU::execDCP(const std::uint16_t addr) {
        const auto val = static_cast<std::uint8_t>(bus_->cpuRead(addr) - 1);
        bus_->cpuWrite(addr, val);
        const auto result = regs_.A - val;
        regs_.updateNZ(result);
        updateBit(regs_.P, CPURegisters::INDEX_C, regs_.A >= val);
    }

    void CPU::execISC(const std::uint16_t addr) {
        const auto m = static_cast<std::uint8_t>(bus_->cpuRead(addr) + 1);
        bus_->cpuWrite(addr, m);
        const auto val = static_cast<int>(regs_.A) - static_cast<int>(m) - (1 - regs_.C());
        const auto result = static_cast<std::uint8_t>(val);
        updateBit(regs_.P, CPURegisters::INDEX_C, val >= 0);
        updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (regs_.A ^ m) & 0x80) != 0);
        regs_.updateNZ(regs_.A = result);
    }

    void CPU::execRLA(const std::uint16_t addr) {
        const auto c = regs_.C();
        auto val = bus_->cpuRead(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 7));
        val = (val << 1) | c;
        bus_->cpuWrite(addr, val);
        regs_.updateNZ(regs_.A &= val);
    }

    void CPU::execRRA(const std::uint16_t addr) {
        const auto c = regs_.C();
        auto val = bus_->cpuRead(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 0));
        val = (val >> 1) | (c << 7);
        bus_->cpuWrite(addr, val);
        const auto sum = static_cast<int>(regs_.A) + static_cast<int>(val) + regs_.C();
        const auto result = static_cast<std::uint8_t>(sum);
        updateBit(regs_.P, CPURegisters::INDEX_C, sum > 255);
        updateBit(regs_.P, CPURegisters::INDEX_V, ((regs_.A ^ result) & (val ^ result) & 0x80) != 0);
        regs_.updateNZ(regs_.A = result);
    }

    void CPU::execSBX(const std::uint16_t addr) {
        const auto val = regs_.A & regs_.X;
        const auto m = bus_->cpuRead(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, val >= m);
        regs_.updateNZ(regs_.X = val - m);
    }

    void CPU::execSLO(const std::uint16_t addr) {
        auto val = bus_->cpuRead(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 7));
        val <<= 1;
        bus_->cpuWrite(addr, val);
        regs_.updateNZ(regs_.A |= val);
    }

    void CPU::execSRE(const std::uint16_t addr) {
        auto val = bus_->cpuRead(addr);
        updateBit(regs_.P, CPURegisters::INDEX_C, getBit(val, 0));
        val >>= 1;
        bus_->cpuWrite(addr, val);
        regs_.updateNZ(regs_.A ^= val);
    }

    void CPU::execXAA(const std::uint16_t addr) {
        regs_.updateNZ(regs_.A = (regs_.A | 0xEE) & regs_.X & bus_->cpuRead(addr));
    }
}
