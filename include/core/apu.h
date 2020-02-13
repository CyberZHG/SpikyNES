#ifndef SPIKYNES_APU_H
#define SPIKYNES_APU_H

#include <array>

#include "core/constants.h"
#include "core/utils.h"

namespace nes_core {

    class Bus;

    struct PulseRegisters {
        std::uint8_t r0 = 0, r1 = 0, r2 = 0, r3 = 0;

        void reset() { r0 = r1 = r2 = r3 = 0; }

        // $4000 / $4004: DDLC VVVV
        std::uint8_t duty() const { return (r0 & 0b11000000) >> 6; }
        bool lengthCounterHalted() const { return getBit(r0, 5); }
        bool constantVolume() const { return getBit(r0, 4); }
        std::uint8_t volume() const { return r0 & 0b00001111; }
        std::uint8_t envelope() const { return r0 & 0b00001111; }

        // $4001 / $4005: EPPP NSSS
        bool sweepEnabled() const { return getBit(r1, 7); }
        std::uint8_t dividerPeriod() const  { return ((r1 & 0b01110000) >> 4) + 1; }
        bool negate() const { return getBit(r1, 3); }
        std::uint8_t shiftCount() const { return r1 & 0b0000111; }

        // $4002 / $4006: TTTT TTTT
        // $4003 / $4007: LLLL LTTT
        std::uint16_t timer() const { return (static_cast<std::uint16_t>(r3 & 0b00000111) << 8) | r2; }
        std::uint8_t lengthCounterLoad() const { return (r3 & 0b11111000) >> 3; }
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
        PulseRegisters pulse1{};
        PulseRegisters pulse2{};
        TriangleRegisters triangle{};
        NoiseRegisters noise{};
        DMCRegisters dmc{};
        std::uint8_t status = 0;
        std::uint8_t frameCounter = 0;

        void reset();
    };

    class APU {
    public:
        explicit APU(Bus* bus);

        void reset();

        std::uint8_t read(std::uint16_t addr) const;
        void write(std::uint16_t addr, std::uint8_t value);

        void step();

    private:
        Bus* bus_ = nullptr;
        APURegisters regs_{};
    };

}

#endif //SPIKYNES_APU_H