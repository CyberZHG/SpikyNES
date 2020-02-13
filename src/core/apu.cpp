#include "core/apu.h"

#include "core/bus.h"

namespace nes_core {
    void APURegisters::reset() {
        pulse1.reset();
        pulse2.reset();
        triangle.reset();
        noise.reset();
        dmc.reset();
        status = 0;
        frameCounter = 0;
    }

    APU::APU(Bus *bus) {
        bus_ = bus;
    }

    void APU::reset() {
        regs_.reset();
    }

    std::uint8_t APU::read(const std::uint16_t addr) const {
        switch (addr) {
            case 0x4015:
                // TODO: return IF-D NT21
                return 0;
            default:
                return bus_->lastCpuRead();
        };
    }

    void APU::write(const std::uint16_t addr, const std::uint8_t value) {
        switch (addr) {
            case 0x4000: regs_.pulse1.r0 = value; break;
            case 0x4001: regs_.pulse1.r1 = value; break;
            case 0x4002: regs_.pulse1.r2 = value; break;
            case 0x4003: regs_.pulse1.r3 = value; break;
            case 0x4004: regs_.pulse2.r0 = value; break;
            case 0x4005: regs_.pulse2.r1 = value; break;
            case 0x4006: regs_.pulse2.r2 = value; break;
            case 0x4007: regs_.pulse2.r3 = value; break;
            case 0x4008: regs_.triangle.r0 = value; break;
            case 0x400A: regs_.triangle.r2 = value; break;
            case 0x400B: regs_.triangle.r3 = value; break;
            case 0x400C: regs_.noise.r0 = value; break;
            case 0x400E: regs_.noise.r2 = value; break;
            case 0x400F: regs_.noise.r3 = value; break;
            case 0x4010: regs_.dmc.r0 = value; break;
            case 0x4011: regs_.dmc.r1 = value; break;
            case 0x4012: regs_.dmc.r2 = value; break;
            case 0x4013: regs_.dmc.r3 = value; break;
            case 0x4015: regs_.status = value; break;
            case 0x4017: regs_.frameCounter = value; break;
            default: break;
        }
    }

    void APU::step() {
    }
}
