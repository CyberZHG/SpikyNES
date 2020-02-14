#ifndef SPIKYNES_APU_H
#define SPIKYNES_APU_H

#include <array>

#include "core/constants.h"
#include "core/utils.h"

namespace nes_core {

    class Bus;

    struct PulseRegisters {

        void reset();
        void write(std::uint16_t addr, std::uint16_t value);
        std::uint8_t output() const;

        void envelopeStep();
        void sweepStep(bool isPulse1);
        void timerStep();
        void lengthCounterStep();

        std::uint8_t duty, dutyBit;
        bool lengthCounterHalted;
        bool constantVolume;
        std::uint8_t volume;

        bool sweepEnabled;
        std::uint8_t sweepDividerPeriod;
        bool sweepNegate;
        std::uint8_t sweepShift;
        bool sweepReload;
        std::uint8_t sweepCounter;

        std::uint16_t timerPeriod;
        std::uint8_t lengthCounterLoad;
        bool envelopeStart;
        std::uint8_t envelopeDecayLevel;
        std::uint8_t envelopeDivider;

        std::uint8_t timer;
        std::uint8_t lengthCounter;
    };

    struct TriangleRegisters {
        std::uint8_t r0 = 0, r2 = 0, r3 = 0;

        void reset() { r0 = r2 = r3 = 0; }

        // $4008: CRRR RRRR
        bool controlFlag() const { return getBit(r0, 7); }
        std::uint8_t counterReload() const { return r0 & 0b01111111; }

        // $400A: LLLL LLLL
        // $400B: llll lHHH
        std::uint16_t timer() const { return (static_cast<std::uint16_t>(r3 & 0b00000111) << 8) | r2; }
        std::uint8_t lengthCounterLoad() const { return (r3 & 0b11111000) >> 3; }
    };

    struct NoiseRegisters {
        std::uint8_t r0 = 0, r2 = 0, r3 = 0;

        void reset() { r0 = r2 = r3 = 0; }

        // $400C: --lc vvvv
        bool lengthCounterHalted() const { return getBit(r0, 5); }
        bool constantVolume() const { return getBit(r0, 4); }
        std::uint8_t volume() const { return r0 & 0b00001111; }
        std::uint8_t envelope() const { return r0 & 0b00001111; }

        // $400E: M--- PPPP
        bool mode() const { return getBit(r2, 7); }
        std::uint8_t periodIndex() const { return r2 & 0b00001111; }
        std::uint8_t lengthCounterLoad() const { return (r3 & 0b11111000) >> 3; }
    };

    struct DMCRegisters {
        std::uint8_t r0 = 0, r1 = 0, r2 = 0, r3 = 0;

        void reset() { r0 = r1 = r2 = r3 = 0; }

        // $4010: IL-- RRRR
        bool irqEnabled() const { return getBit(r0, 7); }
        bool loopEnabled() const { return getBit(r0, 6); }
        std::uint8_t rateIndex() const { return r0 & 0b00001111; }

        // $4011: -DDD DDDD
        std::uint8_t outputLevel() const { return r0 & 0b01111111; }

        // $4012: AAAA AAAA
        std::uint16_t sampleAddress() const { return 0xC000 + (r2 << 6); }

        // $4013: LLLL LLLL
        std::uint16_t sampleLength() const { return (r3 << 4) + 1; }
    };

    struct APURegisters {
        void reset();

        void write(std::uint16_t addr, std::uint16_t value);

        PulseRegisters pulse1{};
        PulseRegisters pulse2{};
        TriangleRegisters triangle{};
        NoiseRegisters noise{};
        DMCRegisters dmc{};

        bool pulse1Enabled;
        bool pulse2Enabled;
        bool triangleEnabled;
        bool noiseEnabled;
        bool dmcEnabled;

        bool sequencerMode;
        bool irqInhibited;
    };

    class APU {
    public:
        explicit APU(Bus* bus);

        void reset();

        std::uint8_t read(std::uint16_t addr) const;
        void write(std::uint16_t addr, std::uint8_t value);

        void step();

        APURegisters& registers() { return regs_; }
        const APURegisters& registers() const { return regs_; }

        float outputBuffer() const { return outputBuffer_; }

    protected:
        void quarterFrameStep();
        void halfFrameStep();

        std::uint8_t pulse1Output() const;
        std::uint8_t pulse2Output() const;
        float mixOutputs() const;

    private:
        Bus* bus_ = nullptr;
        APURegisters regs_{};
        std::uint64_t cycle_ = 0;
        float outputBuffer_ = 0.0;
    };

}

#endif //SPIKYNES_APU_H