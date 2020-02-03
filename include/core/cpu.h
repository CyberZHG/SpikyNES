#ifndef SPIKYNES_CPU_H
#define SPIKYNES_CPU_H

#include <cstdint>

namespace nes_core {

    struct CPURegisters {
        std::int8_t A = 0;
        std::int8_t X = 0;
        std::int8_t Y = 0;
        std::int8_t P = 0b00100100;
        std::int8_t SP = 0;
        std::int16_t PC = 0;

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
        /**
         * Execute one instruction.
         * @return Cycles consumed.
         */
        int step();

    private:
        CPURegisters regs_;
    };

}

#endif //SPIKYNES_CPU_H