#include "core/apu.h"

#include "core/bus.h"

namespace nes_core {
    constexpr std::uint8_t LENGTH_COUNTER_TABLE[32] = {
        10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
        12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30,
    };

    constexpr std::uint8_t TRIANGLE_TABLE[32] = {
        15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    };

    constexpr std::uint16_t NOISE_PERIOD_TABLE[16] = {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068,
    };

    void PulseRegisters::reset() {
        duty = 0;
        dutyBit = 0;
        lengthCounterHalted = false;
        constantVolume = false;
        volume = 0;
        sweepEnabled = false;
        sweepDividerPeriod = 0;
        sweepNegate = false;
        sweepShift = 0;
        sweepReload = false;
        sweepCounter = 0;
        timerPeriod = 0;
        lengthCounterLoad = 0;
        envelopeStart = false;
        envelopeDecayLevel = 0;
        envelopeDivider = 0;
        timer = 0;
        lengthCounter = 0;
    }

    void PulseRegisters::write(const std::uint16_t addr, const std::uint16_t value) {
        switch (addr) {
            case 0x4000:
            case 0x4004:
                // $4000 / $4004: DDLC VVVV
                switch ((value & 0b11000000) >> 6) {
                case 0: duty = 0b01000000; break;
                case 1: duty = 0b01100000; break;
                case 2: duty = 0b01111000; break;
                case 3: duty = 0b10011111; break;
                default: break;
                }
                lengthCounterHalted = getBit(value, 5);
                constantVolume = getBit(value, 4);
                volume = value & 0b00001111;
                break;
            case 0x4001:
            case 0x4005:
                // $4001 / $4005: EPPP NSSS
                sweepEnabled = getBit(value, 7);
                sweepDividerPeriod = ((value & 0b01110000) >> 4) + 1;
                sweepNegate = getBit(value, 3);
                sweepShift = value & 0b00000111;
                sweepReload = true;
                break;
            case 0x4002:
            case 0x4006:
                // $4002 / $4006: TTTT TTTT
                timerPeriod = (timerPeriod & ~0x00FF) | value;
                break;
            case 0x4003:
            case 0x4007:
                // $4003 / $4007: LLLL LTTT
                timerPeriod = (timerPeriod & ~0x0700) | ((value & 0b00000111) << 8);
                lengthCounter = LENGTH_COUNTER_TABLE[value >> 3];
                envelopeStart = true;
                dutyBit = 0;
                break;
            default:
                break;
        }
    }

    std::uint8_t PulseRegisters::output() const {
        if (lengthCounter == 0) {
            return 0;
        }
        if (timerPeriod < 8 || timerPeriod > 0x7FF) {
            return 0;
        }
        if ((duty & (1 << (7 - dutyBit))) == 0) {
            return 0;
        }
        return constantVolume ? volume : envelopeDecayLevel;
    }

    void PulseRegisters::envelopeStep() {
        if (envelopeStart) {
            envelopeStart = false;
            envelopeDecayLevel = 15;
            envelopeDivider = volume;
        } else if (envelopeDivider > 0) {
            --envelopeDivider;
        } else {
            envelopeDivider = volume;
            if (envelopeDecayLevel > 0) {
                --envelopeDecayLevel;
            } else if (lengthCounterHalted) {
                envelopeDecayLevel = 15;
            }
        }
    }

    void PulseRegisters::sweepStep(const bool isPulse1) {
        if (sweepEnabled && sweepCounter == 0 && sweepShift > 0 && timerPeriod >= 8) {
            auto update = timerPeriod >> sweepShift;
            if (sweepNegate) {
                update = -update;
                if (isPulse1) {
                    --update;
                }
            }
            timerPeriod += update;
        }
        if (sweepCounter == 0 || sweepReload) {
            sweepCounter = sweepDividerPeriod;
            sweepReload = false;
        } else {
            --sweepCounter;
        }
    }

    void PulseRegisters::timerStep() {
        if (timer == 0) {
            timer = timerPeriod;
            dutyBit = (dutyBit + 1) % 8;
        } else {
            --timer;
        }
    }

    void PulseRegisters::lengthCounterStep() {
        if (!lengthCounterHalted && lengthCounter > 0) {
            --lengthCounter;
        }
    }

    void TriangleRegisters::reset() {
        lengthCounterHalted = false;
        linearCounterReload = 0;
        linearCounter = 0;
        timerPeriod = 0;
        lengthCounterLoad = 0;
        linearStart = false;
        dutyIndex = 0;
        timer = 0;
        lengthCounter = 0;
    }

    void TriangleRegisters::write(const std::uint16_t addr, const std::uint16_t value) {
        switch (addr) {
            case 0x4008:
                // $4008: CRRR RRRR
                lengthCounterHalted = getBit(value, 7);
                linearCounterReload = value & 0b01111111;
                break;
            case 0x400A:
                // $400A: LLLL LLLL
                timerPeriod = (timerPeriod & ~0x00FF) | value;
                break;
            case 0x400B:
                // $400B: llll lHHH
                timerPeriod = (timerPeriod & ~0x0700) | ((value & 0b00000111) << 8);
                lengthCounter = LENGTH_COUNTER_TABLE[value >> 3];
                linearStart = true;
                break;
            default:
                break;
        }
    }

    std::uint8_t TriangleRegisters::output() const {
        if (lengthCounter == 0 || linearCounter == 0) {
            return 0;
        }
        return TRIANGLE_TABLE[dutyIndex];
    }

    void TriangleRegisters::linearCounterStep() {
        if (linearStart) {
            linearCounter = linearCounterReload;
        } else if (linearCounter > 0) {
            --linearCounter;
        }
        if (!lengthCounterHalted) {
            linearStart = false;
        }
    }

    void TriangleRegisters::timerStep() {
        if (timer == 0) {
            timer = timerPeriod;
            if (lengthCounter > 0 && linearCounter > 0) {
                dutyIndex = (dutyIndex + 1) % 32;
            }
        } else {
            --timer;
        }
    }

    void TriangleRegisters::lengthCounterStep() {
        if (!lengthCounterHalted && lengthCounter > 0) {
            --lengthCounter;
        }
    }

    void NoiseRegisters::reset() {
        lengthCounterHalted = false;
        constantVolume = false;
        volume = 0;
        mode = false;
        periodIndex = 0;
        shiftRegister = 1;
        envelopeStart = false;
        envelopeDecayLevel = 0;
        envelopeDivider = 0;
        timer = 0;
        lengthCounter = 0;
    }

    void NoiseRegisters::write(const std::uint16_t addr, const std::uint16_t value) {
        switch (addr) {
            case 0x400C:
                // $400C: --lc vvvv
                lengthCounterHalted = getBit(value, 5);
                constantVolume = getBit(value, 4);
                volume = value & 0b00001111;
                break;
            case 0x400E:
                // $400E: M--- PPPP
                mode = getBit(value, 7);
                periodIndex = value & 0b00001111;
                break;
            case 0x400F:
                // $400F: llll l---
                lengthCounter = LENGTH_COUNTER_TABLE[value >> 3];
                envelopeStart = true;
                break;
            default:
                break;
        }
    }

    std::uint8_t NoiseRegisters::output() const {
        if (lengthCounter == 0 || (shiftRegister & 1)) {
            return 0;
        }
        return constantVolume ? volume : envelopeDecayLevel;
    }

    void NoiseRegisters::envelopeStep() {
        if (envelopeStart) {
            envelopeStart = false;
            envelopeDecayLevel = 15;
            envelopeDivider = volume;
        } else if (envelopeDivider > 0) {
            --envelopeDivider;
        } else {
            envelopeDivider = volume;
            if (envelopeDecayLevel > 0) {
                --envelopeDecayLevel;
            } else if (lengthCounterHalted) {
                envelopeDecayLevel = 15;
            }
        }
    }

    void NoiseRegisters::timerStep() {
        if (timer == 0) {
            timer = NOISE_PERIOD_TABLE[periodIndex];
            const std::uint16_t otherBit = mode ? ((shiftRegister >> 6) & 1) : ((shiftRegister >> 1) & 1);
            const std::uint16_t feedback = (shiftRegister & 1) ^ otherBit;
            shiftRegister = (shiftRegister >> 1) | (feedback << 14);
        } else {
            --timer;
        }
    }

    void NoiseRegisters::lengthCounterStep() {
        if (!lengthCounterHalted && lengthCounter > 0) {
            --lengthCounter;
        }
    }

    void APURegisters::reset() {
        pulse1.reset();
        pulse2.reset();
        triangle.reset();
        noise.reset();
        dmc.reset();

        pulse1Enabled = false;
        pulse2Enabled = false;
        triangleEnabled = false;
        noiseEnabled = false;
        dmcEnabled = false;

        sequencerMode = false;
        irqInhibited = false;
    }

    void APURegisters::write(const std::uint16_t addr, const std::uint16_t value) {
        switch (addr) {
            case 0x4015:
                pulse1Enabled = getBit(value, 0);
                pulse2Enabled = getBit(value, 1);
                triangleEnabled = getBit(value, 2);
                noiseEnabled = getBit(value, 3);
                dmcEnabled = getBit(value, 4);
                break;
            case 0x4017:
                sequencerMode = getBit(value, 7);
                irqInhibited = getBit(value, 6);
                break;
            default: break;
        }
    }

    APU::APU(Bus *bus) {
        bus_ = bus;
    }

    void APU::reset() {
        regs_.reset();
        cycle_ = 0;
        outputBuffer_ = 0.0;
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
            case 0x4000:
            case 0x4001:
            case 0x4002:
            case 0x4003:
                regs_.pulse1.write(addr, value);
                break;
            case 0x4004:
            case 0x4005:
            case 0x4006:
            case 0x4007:
                regs_.pulse2.write(addr, value);
                break;
            case 0x4008:
            case 0x400A:
            case 0x400B:
                regs_.triangle.write(addr, value);
                break;
            case 0x400C:
            case 0x400E:
            case 0x400F:
                regs_.noise.write(addr, value);
                break;
            case 0x4010: regs_.dmc.r0 = value; break;
            case 0x4011: regs_.dmc.r1 = value; break;
            case 0x4012: regs_.dmc.r2 = value; break;
            case 0x4013: regs_.dmc.r3 = value; break;
            case 0x4015:
            case 0x4017:
                regs_.write(addr, value);
                // TODO: Reset after some CPU cycles
                cycle_ = 0;
                if (regs_.sequencerMode) {
                    quarterFrameStep();
                    halfFrameStep();
                }
                break;
            default: break;
        }
    }

    void APU::step() {
        ++cycle_;
        if (!regs_.sequencerMode) {
            // 4-step sequence
            switch (cycle_ % 14915) {
                case 3728:
                    quarterFrameStep();
                    break;
                case 7456:
                    quarterFrameStep();
                    halfFrameStep();
                    break;
                case 11185:
                    quarterFrameStep();
                    break;
                case 14914:
                    quarterFrameStep();
                    halfFrameStep();
                    if (!regs_.irqInhibited) {
                        // bus_.irq();
                    }
                    break;
                default: break;
            }
        } else {
            // 5-step sequence
            switch (cycle_ % 18641) {
                case 3728:
                    quarterFrameStep();
                    break;
                case 7456:
                    quarterFrameStep();
                    halfFrameStep();
                    break;
                case 11185:
                    quarterFrameStep();
                    break;
                case 18640:
                    quarterFrameStep();
                    halfFrameStep();
                    break;
                default: break;
            }
        }
        regs_.pulse1.timerStep();
        regs_.pulse2.timerStep();
        regs_.triangle.timerStep();
        regs_.noise.timerStep();
        // dmc: timer
        outputBuffer_ = mixOutputs();
    }

    void APU::quarterFrameStep() {
        regs_.pulse1.envelopeStep();
        regs_.pulse2.envelopeStep();
        regs_.triangle.linearCounterStep();
        regs_.noise.envelopeStep();
    }

    void APU::halfFrameStep() {
        regs_.pulse1.lengthCounterStep();
        regs_.pulse1.sweepStep(true);
        regs_.pulse2.lengthCounterStep();
        regs_.pulse2.sweepStep(false);
        regs_.triangle.lengthCounterStep();
        regs_.noise.lengthCounterStep();
    }

    std::uint8_t APU::pulse1Output() const {
        return regs_.pulse1Enabled ? regs_.pulse1.output() : 0;
    }

    std::uint8_t APU::pulse2Output() const {
        return regs_.pulse2Enabled ? regs_.pulse2.output() : 0;
    }

    float APU::mixOutputs() const {
        const auto pulse1 = pulse1Output();
        const auto pulse2 = pulse2Output();
        // TODO: triangle, noise, dmc
        float pulseOutput = 0.0;
        if (pulse1 + pulse2 > 0) {
            pulseOutput = 95.52f / (8128.0f / (static_cast<float>(pulse1) + pulse2) + 100.0f);
        }
        return pulseOutput;
    }
}
