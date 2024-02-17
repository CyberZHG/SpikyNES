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
        [[nodiscard]] std::uint8_t output() const;

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

        std::uint16_t timer;
        std::uint8_t lengthCounter;
    };

    struct TriangleRegisters {
        void reset();
        void write(std::uint16_t addr, std::uint16_t value);
        [[nodiscard]] std::uint8_t output() const;

        void linearCounterStep();
        void timerStep();
        void lengthCounterStep();

        bool lengthCounterHalted;
        std::uint8_t linearCounterReload;
        std::uint8_t linearCounter;

        std::uint16_t timerPeriod;
        std::uint8_t lengthCounterLoad;
        bool linearStart;

        std::uint8_t dutyIndex;
        std::uint16_t timer;
        std::uint8_t lengthCounter;
    };

    struct NoiseRegisters {
        void reset();
        void write(std::uint16_t addr, std::uint16_t value);
        [[nodiscard]] std::uint8_t output() const;

        void envelopeStep();
        void timerStep();
        void lengthCounterStep();

        bool lengthCounterHalted;
        bool constantVolume;
        std::uint8_t volume;

        bool mode;
        std::uint8_t periodIndex;
        std::uint16_t shiftRegister;

        bool envelopeStart;
        std::uint8_t envelopeDecayLevel;
        std::uint8_t envelopeDivider;

        std::uint16_t timer;
        std::uint8_t lengthCounter;
    };

    struct DMCRegisters {
        void reset();
        void write(std::uint16_t addr, std::uint16_t value);
        [[nodiscard]] std::uint8_t output() const;

        void timerStep();
        void restart();

        bool irqEnabled;
        bool loopEnabled;
        std::uint8_t rateIndex;

        std::uint8_t outputLevel;

        std::uint8_t sampleAddress;
        std::uint8_t sampleLength;

        std::uint16_t currentAddress;
        std::uint16_t bytesRemaining;
        std::uint8_t sampleBuffer;
        bool sampleBufferEmpty;

        std::uint8_t shiftRegister;
        std::uint8_t bitsRemaining;
        bool silence;

        bool irqFlag;
        std::uint16_t timer;
    };

    struct APURegisters {
        void reset();

        void write(std::uint16_t addr, std::uint16_t value);

        PulseRegisters pulse1{};
        PulseRegisters pulse2{};
        TriangleRegisters triangle{};
        NoiseRegisters noise{};
        DMCRegisters dmc{};

        bool pulse1Enabled = false;
        bool pulse2Enabled = false;
        bool triangleEnabled = false;
        bool noiseEnabled = false;
        bool dmcEnabled = false;

        bool sequencerMode = false;
        bool irqInhibited = false;
        bool frameIrqFlag = false;
    };

    class APU {
    public:
        explicit APU(Bus* bus);

        void reset();

        [[nodiscard]] std::uint8_t read(std::uint16_t addr);
        void write(std::uint16_t addr, std::uint8_t value);

        void step();

        APURegisters& registers() { return regs_; }
        [[nodiscard]] const APURegisters& registers() const { return regs_; }

        [[nodiscard]] float outputBuffer() const { return outputBuffer_; }

        [[nodiscard]] bool irqAsserted() const { return regs_.frameIrqFlag || regs_.dmc.irqFlag; }

    protected:
        void quarterFrameStep();
        void halfFrameStep();

        [[nodiscard]] std::uint8_t pulse1Output() const;
        [[nodiscard]] std::uint8_t pulse2Output() const;
        [[nodiscard]] std::uint8_t triangleOutput() const;
        [[nodiscard]] std::uint8_t noiseOutput() const;
        [[nodiscard]] std::uint8_t dmcOutput() const;
        [[nodiscard]] float mixOutputs() const;

    private:
        Bus* bus_ = nullptr;
        APURegisters regs_{};
        std::uint64_t cycle_ = 0;
        float outputBuffer_ = 0.0;
        int frameResetCountdown_ = -1;
    };

}

#endif //SPIKYNES_APU_H