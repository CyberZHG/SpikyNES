#ifndef SPIKYNES_CPU_H
#define SPIKYNES_CPU_H

#include <array>
#include <optional>

#include "core/constants.h"
#include "core/utils.h"

namespace nes_core {

    class Bus;

    struct CPURegisters {
        static constexpr std::uint8_t DEFAULT_STATE = 0b00100100;

        std::uint8_t A = 0;
        std::uint8_t X = 0;
        std::uint8_t Y = 0;
        std::uint8_t P = DEFAULT_STATE;
        std::uint8_t SP = 0;
        std::uint16_t PC = 0;

        static constexpr int INDEX_N = 7;
        static constexpr int INDEX_V = 6;
        static constexpr int INDEX_B = 4;
        static constexpr int INDEX_D = 3;
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
        [[nodiscard]] bool N() const { return getBit(P, INDEX_N); }
        /**
         * @return V (overflow) flag.
         */
        [[nodiscard]] bool V() const { return getBit(P, INDEX_V); }
        /**
         * B is 0 when pushed by interrupts (NMI and IRQ) and 1 when pushed by instructions (BRK and PHP).
         * @return B flag.
         */
        [[nodiscard]] bool B() const { return getBit(P, INDEX_B); }
        /**
         * @return I (interrupt disable) flag.
         */
        [[nodiscard]] bool I() const { return getBit(P, INDEX_I); }
        /**
         * @return Z (zero) flag.
         */
        [[nodiscard]] bool Z() const { return getBit(P, INDEX_Z); }
        /**
         * @return C (carry) flag.
         */
        [[nodiscard]] bool C() const { return getBit(P, INDEX_C); }
    };

    struct AddressingResult {
        std::uint16_t addr = 0;
        bool pageCrossed = false;
    };

    struct OperationExecutionState {
        bool executingOp = false;
        std::uint8_t code = 0;
        std::uint8_t step = 0;
        std::uint8_t readLo = 0;
        std::uint8_t readHi = 0;
        std::uint8_t readDummy = 0;
        std::uint16_t baseAddr = 0;
        std::uint16_t addr = 0;
        bool hasPageCross = false;
        std::uint8_t addressingFinishedStep = 0xFF;
        bool finished = false;
        bool isImplied = false;
        bool isStore = false;

        void reset();
    };

    class CPU {
    public:
        explicit CPU(Bus* bus);

        void reset();

        CPURegisters& registers() { return regs_; }
        [[nodiscard]] const CPURegisters& registers() const { return regs_; }

        [[nodiscard]] std::uint8_t cpuRead(std::uint16_t addr) const;
        void cpuWrite(std::uint16_t addr, std::uint16_t value);

        /**
         * Execute one instruction.
         * @return Cycles consumed.
         */
        int step();
        [[nodiscard]] std::int64_t cycles() const { return cycles_; }
        void addCycles(const std::int64_t cycles) { cycles_ += cycles; }
        [[nodiscard]] std::int64_t steps() const { return steps_; }

        /** Check if CPU is halted by JAM/KIL instruction. */
        [[nodiscard]] bool halted() const { return halted_; }

        /** Trigger a Non-Maskable Interrupt. */
        void nmi() { nmiPending_ = true; }
        /** Check if NMI is pending. */
        [[nodiscard]] bool nmiPending() const { return nmiPending_; }
        /** Trigger an Interrupt Request (ignored if I flag is set). */
        void irq() { irqPending_ = true; }

        /** Trigger OAM DMA transfer. */
        void triggerDMA(std::uint8_t page);
        /** Check if DMA is in progress. */
        [[nodiscard]] bool dmaPending() const { return dmaPending_; }

        /** Stall the CPU for the given number of cycles. */
        void stall(const int cycles) { stallCycles_ += cycles; }

    protected:
        /** Push a byte onto the stack. */
        void pushByte(std::uint8_t value);
        /** Push a word onto the stack (high byte first). */
        void pushWord(std::uint16_t value);
        /** Pop a byte from the stack. */
        std::uint8_t popByte();
        /** Pop a word from the stack (low byte first). */
        std::uint16_t popWord();

        /** The operand is contained in the second byte of the instruction. */
        void addrImmediate();
        /** The second byte as low order bits and the third byte as the high order bits. */
        void addrAbsolute();
        /** X-indexed absolute. */
        void addrAbsoluteX();
        /** Y-indexed absolute. */
        void addrAbsoluteY();
        /** Use the address from memory. */
        void addrAbsoluteIndirect();
        /** Fetch from $00 - $FF. */
        void addrZeroPage();
        /** X-indexed zero page. */
        void addrZeroPageX();
        /** Y-indexed zero page. */
        void addrZeroPageY();
        /** Use the address from X-indexed zero page. */
        void addrZeroPageIndirectX();
        /** Use the address from zero page then plus Y. */
        void addrZeroPageIndirectY();
        /** Add offset to the program counter. */
        void addrRelative();

        /** No operation. */
        void execNOP();

        /** Load accumulator with memory. */
        void execLDA();
        /** Load index register X from memory. */
        void execLDX();
        /** Load index register Y from memory. */
        void execLDY();

        /** Store accumulator in memory. */
        void execSTA();
        /** Store index register X in memory. */
        void execSTX();
        /** Store index register Y in memory. */
        void execSTY();

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

        /** Arithmetic shift left. */
        void execASL();
        /** Logical shift right. */
        void execLSR();

        /** Rotate left. */
        void execROL();
        /** Rotate right. */
        void execROR();

        /** "AND" memory with accumulator. */
        void execAND();
        /** "OR" memory with accumulator. */
        void execORA();
        /** "XOR" memory with accumulator. */
        void execEOR();
        /** Test bits in memory with accumulator. */
        void execBIT();

        /** Add memory to accumulator with carry. */
        void execADC();
        /** Subtract memory from accumulator with borrow. */
        void execSBC();
        /** Compare memory and accumulator. */
        void execCMP();
        /** Compare index register X to memory. */
        void execCPX();
        /** Compare index register Y to memory. */
        void execCPY();

        /** Increment memory by one. */
        void execINC();
        /** Increment index register X by one. */
        void execINX();
        /** Increment index register Y by one. */
        void execINY();
        /** Decrement memory by one. */
        void execDEC();
        /** Decrement index register X by one. */
        void execDEX();
        /** Decrement index register Y by one. */
        void execDEY();

        /** Jump indirect. */
        void execJMP();
        /** Jump to subroutine. */
        void execJSR();
        /** Return from subroutine. */
        void execRTS();

        /** Break command. */
        void execBRK();
        /** Return from interrupt. */
        void execRTI();

        /** Branch on carry clear. */
        void execBCC();
        /** Branch on carry set. */
        void execBCS();
        /** Branch on result zero. */
        void execBEQ();
        /** Branch on result minus. */
        void execBMI();
        /** Branch on result not zero. */
        void execBNE();
        /** Branch on result plus. */
        void execBPL();
        /** Branch on overflow clear. */
        void execBVC();
        /** Branch on overflow set. */
        void execBVS();

        /** Clear carry flag. */
        void execCLC();
        /** Clear decimal mode. */
        void execCLD();
        /** Clear interrupt disable. */
        void execCLI();
        /** Clear overflow flag. */
        void execCLV();
        /** Set carry flag. */
        void execSEC();
        /** Set decimal mode. */
        void execSED();
        /** set interrupt disable. */
        void execSEI();

        // Unofficial opcodes
        /** "AND" memory with stack pointer. */
        void execLAS();
        /** Load accumulator and index register X from memory. */
        void execLAX();
        /** Store accumulator "AND" index register X in memory. */
        void execSAX();
        /** Store accumulator "AND" index register X "AND" value. */
        void execSHA();
        /** Store index register X "AND" value. */
        void execSHX();
        /** Store index register Y "AND" value. */
        void execSHY();
        /** Transfer accumulator "AND" index register X to stack pointer then store stack pointer "AND" hi-byte in memory. */
        void execSHS();
        /** "AND" memory with accumulator then move negative flag to carry flag. */
        void execANC();
        /** "AND" accumulator then rotate right. */
        void execARR();
        /** "AND" then logical shift right. */
        void execASR();
        /** Decrement memory by one then compare with accumulator */
        void execDCP();
        /** Increment memory by one then SBC then subtract memory from accumulator with borrow. */
        void execISC();
        /** Rotate left then "AND" with accumulator. */
        void execRLA();
        /** Rotate right and add memory to accumulator. */
        void execRRA();
        /** Subtract memory from accumulator "AND" index register X. */
        void execSBX();
        /** Arithmetic shift left then "OR" memory with accumulator. */
        void execSLO();
        /** Logical shift right then "XOR" memory with accumulator. */
        void execSRE();
        /** Non-deterministic operation of accumulator, index register X, memory and bus contents. */
        void execXAA();
        /** Halt the CPU (JAM/KIL). */
        void execJAM();

        /** Handle NMI interrupt. */
        void handleNMI();
        /** Handle IRQ interrupt. */
        void handleIRQ();

    private:
        /**
         * 2KB internal RAM, mirrored 3 times and up to $2000.
         */
        std::array<std::uint8_t, CPU_RAM_SIZE> mem_ = {};
        CPURegisters regs_;
        Bus* bus_ = nullptr;

        OperationExecutionState opState_;

        std::int64_t cycles_ = 0;
        std::int64_t steps_ = 0;
        int stallCycles_ = 0;
        bool halted_ = false;
        bool nmiPending_ = false;
        bool nmiActive_ = false;
        bool irqPending_ = false;
        bool irqActive_ = false;
        bool dmaPending_ = false;
        std::uint8_t dmaPage_ = 0;
        std::int32_t dmaIndex_ = 0;
        std::uint8_t dmaByte_ = 0;
        bool dmaAlignCycle_ = false;
    };

}

#endif //SPIKYNES_CPU_H