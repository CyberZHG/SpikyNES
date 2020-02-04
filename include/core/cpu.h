#ifndef SPIKYNES_CPU_H
#define SPIKYNES_CPU_H

#include <array>

#include "core/constants.h"
#include "core/utils.h"

namespace nes_core {

    class Bus;

    struct CPURegisters {
        static constexpr std::uint8_t DEFAULT_STATE = 0b00110100;

        std::uint8_t A = 0;
        std::uint8_t X = 0;
        std::uint8_t Y = 0;
        std::uint8_t P = DEFAULT_STATE;
        std::uint8_t SP = 0;
        std::uint16_t PC = 0;

        static constexpr int INDEX_N = 7;
        static constexpr int INDEX_V = 6;
        static constexpr int INDEX_B = 4;
        static constexpr int INDEX_I = 2;
        static constexpr int INDEX_Z = 1;
        static constexpr int INDEX_C = 0;

        void reset();

        void updateN(std::uint8_t val);
        void updateZ(std::uint8_t val);
        void updateNZ(std::uint8_t val);

        /**
         * @return N (negative) flag.
         */
        bool N() const { return getBit(P, INDEX_N); }
        /**
         * @return V (overflow) flag.
         */
        bool V() const { return getBit(P, INDEX_V); }
        /**
         * B is 0 when pushed by interrupts (NMI and IRQ) and 1 when pushed by instructions (BRK and PHP).
         * @return B flag.
         */
        bool B() const { return getBit(P, INDEX_B); }
        /**
         * @return I (interrupt disable) flag.
         */
        bool I() const { return getBit(P, INDEX_I); }
        /**
         * @return Z (zero) flag.
         */
        bool Z() const { return getBit(P, INDEX_Z); }
        /**
         * @return C (carry) flag.
         */
        bool C() const { return getBit(P, INDEX_C); }
    };

    struct AddressingResult {
        std::uint16_t addr = 0;
        bool pageCrossed = false;
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

        /** The operand is contained in the second byte of the instruction. */
        AddressingResult addrImmediate();
        /** The second byte as low order bits and the third byte as the high order bits. */
        AddressingResult addrAbsolute();
        /** X-indexed absolute. */
        AddressingResult addrAbsoluteX();
        /** Y-indexed absolute. */
        AddressingResult addrAbsoluteY();
        /** Fetch from $00 - $FF. */
        AddressingResult addrZeroPage();
        /** X-indexed zero page. */
        AddressingResult addrZeroPageX();
        /** Y-indexed zero page. */
        AddressingResult addrZeroPageY();
        /** Use the address from X-indexed zero page. */
        AddressingResult addrZeroPageIndirectX();
        /** Use the address from zero page then plus Y. */
        AddressingResult addrZeroPageIndirectY();

        /** Load accumulator with memory. */
        void execLDA(std::uint16_t addr);
        /** Load index register X from memory. */
        void execLDX(std::uint16_t addr);
        /** Load index register Y from memory. */
        void execLDY(std::uint16_t addr);

        /** Store accumulator in memory. */
        void execSTA(std::uint16_t addr) const;
        /** Store index register X in memory. */
        void execSTX(std::uint16_t addr) const;
        /** Store index register Y in memory. */
        void execSTY(std::uint16_t addr) const;

        /** Transfer accumulator to index X. */
        void execTAX();
        /** Transfer accumulator to index Y. */
        void execTAY();
        /** Transfer index X to accumulator. */
        void execTXA();
        /** Transfer index Y to accumulator. */
        void execTYA();
        /** Transfer stack pointer to index X. */
        void execTSX();
        /** Transfer index X to stack pointer. */
        void execTXS();

        /** Push accumulator on stack. */
        void execPHA();
        /** Push processor status on stack. */
        void execPHP();
        /** Pull accumulator from stack. */
        void execPLA();
        /** Pull processor status from stack. */
        void execPLP();

        /** Arithmetic shift left on accumulator. */
        void execASL();
        /** Arithmetic shift left. */
        void execASL(std::uint16_t addr);
        /** Logical shift right on accumulator. */
        void execLSR();
        /** Logical shift right. */
        void execLSR(std::uint16_t addr);

        /** Rotate left on accumulator. */
        void execROL();
        /** Rotate left. */
        void execROL(std::uint16_t addr);
        /** Rotate right on accumulator. */
        void execROR();
        /** Rotate right. */
        void execROR(std::uint16_t addr);

        /** "AND" memory with accumulator. */
        void execAND(std::uint16_t addr);
        /** "OR" memory with accumulator. */
        void execORA(std::uint16_t addr);
        /** "XOR" memory with accumulator. */
        void execEOR(std::uint16_t addr);
        /** Test bits in memory with accumulator. */
        void execBIT(std::uint16_t addr);

        /** Add memory to accumulator with carry. */
        void execADC(std::uint16_t addr);
        /** Subtract memory from accumulator with borrow. */
        void execSBC(std::uint16_t addr);
        /** Compare memory and accumulator. */
        void execCMP(std::uint16_t addr);
        /** Compare index register X to memory. */
        void execCPX(std::uint16_t addr);
        /** Compare index register Y to memory. */
        void execCPY(std::uint16_t addr);

        /** Increment memory by one. */
        void execINC(std::uint16_t addr);
        /** Increment index register X by one. */
        void execINX();
        /** Increment index register Y by one. */
        void execINY();
        /** Decrement memory by one. */
        void execDEC(std::uint16_t addr);
        /** Decrement index register X by one. */
        void execDEX();
        /** Decrement index register Y by one. */
        void execDEY();

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