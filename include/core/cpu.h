#ifndef SPIKYNES_CPU_H
#define SPIKYNES_CPU_H

#include <array>

#include "core/constants.h"

namespace nes_core {

    class Bus;

    struct CPURegisters {
        std::uint8_t A = 0;
        std::uint8_t X = 0;
        std::uint8_t Y = 0;
        std::uint8_t P = 0b00100100;
        std::uint8_t SP = 0;
        std::uint16_t PC = 0;

        void reset();

        /**
         * @return N (negative) flag.
         */
        bool isNegative() const { return P & 0b10000000; }
        /**
         * @return V (overflow) flag.
         */
        bool isOverflow() const { return P & 0b01000000; }
        /**
         * B is 0 when pushed by interrupts (NMI and IRQ) and 1 when pushed by instructions (BRK and PHP).
         * @return B flag.
         */
        bool isBreak() const { return P & 0b00010000; }
        /**
         * @return I (interrupt disable) flag.
         */
        bool isInterruptDisabled() const { return P & 0b00000100; }
        /**
         * @return Z (zero) flag.
         */
        bool isZero() const { return P & 0b00000010; }
        /**
         * @return C (carry) flag.
         */
        bool isCarry() const { return P & 0b00000001; }
    };

    class CPU {
    public:
        explicit CPU(Bus* bus);

        void reset();

        CPURegisters& registers() { return regs_; }
        const CPURegisters& registers() const { return regs_; }

        /** Read a byte from CPU RAM. */
        std::uint8_t read(std::uint16_t addr) const;
        /** Set a byte to CPU RAM. */
        void write(std::uint16_t addr, std::uint16_t value);

        /**
         * Execute one instruction.
         * @return Cycles consumed.
         */
        int step();

    private:
        /**
         * 2KB internal RAM, mirrored 3 times and up to $2000.
         */
        std::array<std::uint8_t, CPU_RAM_SIZE> mem_ = {};
        CPURegisters regs_;
        Bus* bus_ = nullptr;
    };

}

#endif //SPIKYNES_CPU_H