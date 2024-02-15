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
        [[nodiscard]] bool isHalted() const { return halted_; }

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
        void addrImmediateStep();
        /** The second byte as low order bits and the third byte as the high order bits. */
        void addrAbsoluteStep();
        /** X-indexed absolute. */
        void addrAbsoluteXStep();
        /** Y-indexed absolute. */
        void addrAbsoluteYStep();
        /** Use the address from memory. */
        void addrAbsoluteIndirectStep();
        /** Fetch from $00 - $FF. */
        void addrZeroPageStep();
        /** X-indexed zero page. */
        void addrZeroPageXStep();
        /** Y-indexed zero page. */
        void addrZeroPageYStep();
        /** Use the address from X-indexed zero page. */
        void addrZeroPageIndirectXStep();
        /** Use the address from zero page then plus Y. */
        void addrZeroPageIndirectYStep();
        /** Add offset to the program counter. */
        void addrRelativeStep();

        /** No operation. */
        void execNOPStep();

        /** Load accumulator with memory. */
        void execLDAStep();
        /** Load index register X from memory. */
        void execLDXStep();
        /** Load index register Y from memory. */
        void execLDYStep();

        /** Store accumulator in memory. */
        void execSTAStep();
        /** Store index register X in memory. */
        void execSTXStep();
        /** Store index register Y in memory. */
        void execSTYStep();

        /** Transfer accumulator to index X. */
        void execTAXStep();
        /** Transfer accumulator to index Y. */
        void execTAYStep();
        /** Transfer index X to accumulator. */
        void execTXAStep();
        /** Transfer index Y to accumulator. */
        void execTYAStep();
        /** Transfer stack pointer to index X. */
        void execTSXStep();
        /** Transfer index X to stack pointer. */
        void execTXSStep();

        /** Push accumulator on stack. */
        void execPHAStep();
        /** Push processor status on stack. */
        void execPHPStep();
        /** Pull accumulator from stack. */
        void execPLAStep();
        /** Pull processor status from stack. */
        void execPLPStep();

        /** Arithmetic shift left. */
        void execASLStep();
        /** Logical shift right. */
        void execLSRStep();

        /** Rotate left. */
        void execROLStep();
        /** Rotate right. */
        void execRORStep();

        /** "AND" memory with accumulator. */
        void execANDStep();
        /** "OR" memory with accumulator. */
        void execORAStep();
        /** "XOR" memory with accumulator. */
        void execEORStep();
        /** Test bits in memory with accumulator. */
        void execBITStep();

        /** Add memory to accumulator with carry. */
        void execADCStep();
        /** Subtract memory from accumulator with borrow. */
        void execSBCStep();
        /** Compare memory and accumulator. */
        void execCMPStep();
        /** Compare index register X to memory. */
        void execCPXStep();
        /** Compare index register Y to memory. */
        void execCPYStep();

        /** Increment memory by one. */
        void execINCStep();
        /** Increment index register X by one. */
        void execINXStep();
        /** Increment index register Y by one. */
        void execINYStep();
        /** Decrement memory by one. */
        void execDECStep();
        /** Decrement index register X by one. */
        void execDEXStep();
        /** Decrement index register Y by one. */
        void execDEYStep();

        /** Jump indirect. */
        void execJMPStep();
        /** Jump to subroutine. */
        void execJSRStep();
        /** Return from subroutine. */
        void execRTSStep();

        /** Break command. */
        void execBRKStep();
        /** Return from interrupt. */
        void execRTIStep();

        /** Branch on carry clear. */
        void execBCCStep();
        /** Branch on carry set. */
        void execBCSStep();
        /** Branch on result zero. */
        void execBEQStep();
        /** Branch on result minus. */
        void execBMIStep();
        /** Branch on result not zero. */
        void execBNEStep();
        /** Branch on result plus. */
        void execBPLStep();
        /** Branch on overflow clear. */
        void execBVCStep();
        /** Branch on overflow set. */
        void execBVSStep();

        /** Clear carry flag. */
        void execCLCStep();
        /** Clear decimal mode. */
        void execCLDStep();
        /** Clear interrupt disable. */
        void execCLIStep();
        /** Clear overflow flag. */
        void execCLVStep();
        /** Set carry flag. */
        void execSECStep();
        /** Set decimal mode. */
        void execSEDStep();
        /** set interrupt disable. */
        void execSEIStep();

        // Unofficial opcodes
        /** "AND" memory with stack pointer. */
        void execLASStep();
        /** Load accumulator and index register X from memory. */
        void execLAXStep();
        /** Store accumulator "AND" index register X in memory. */
        void execSAXStep();
        /** Store accumulator "AND" index register X "AND" value. */
        void execSHAStep();
        /** Store index register X "AND" value. */
        void execSHXStep();
        /** Store index register Y "AND" value. */
        void execSHYStep();
        /** Transfer accumulator "AND" index register X to stack pointer then store stack pointer "AND" hi-byte in memory. */
        void execSHSStep();
        /** "AND" memory with accumulator then move negative flag to carry flag. */
        void execANCStep();
        /** "AND" accumulator then rotate right. */
        void execARRStep();
        /** "AND" then logical shift right. */
        void execASRStep();
        /** Decrement memory by one then compare with accumulator */
        void execDCPStep();
        /** Increment memory by one then SBC then subtract memory from accumulator with borrow. */
        void execISCStep();
        /** Rotate left then "AND" with accumulator. */
        void execRLAStep();
        /** Rotate right and add memory to accumulator. */
        void execRRAStep();
        /** Subtract memory from accumulator "AND" index register X. */
        void execSBXStep();
        /** Arithmetic shift left then "OR" memory with accumulator. */
        void execSLOStep();
        /** Logical shift right then "XOR" memory with accumulator. */
        void execSREStep();
        /** Non-deterministic operation of accumulator, index register X, memory and bus contents. */
        void execXAAStep();
        /** Halt the CPU (JAM/KIL). */
        void execJAMStep();

        /** Handle NMI interrupt. */
        void handleNMIStep();
        /** Handle IRQ interrupt. */
        void handleIRQStep();

    private:
        /**
         * 2KB internal RAM, mirrored 3 times and up to $2000.
         */
        std::array<std::uint8_t, CPU_RAM_SIZE> mem_ = {};
        CPURegisters regs_;
        Bus* bus_ = nullptr;

        bool isHighPrecision_ = false;
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
        bool dmaAlignCycle_ = false;
    };

}

#endif //SPIKYNES_CPU_H