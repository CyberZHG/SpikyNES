#include "core/bus.h"
#include "core/cpu.h"

namespace nes_core {
    enum class OpCode {
        // Load/Store
        LDA, LDX, LDY,
        STA, STX, STY,
        // Transfers
        TAX, TAY, TXA, TYA, TSX, TXS,
        // Stacks
        PHA, PHP, PLA, PLP,
        // Shifts
        ASL, LSR, ROL, ROR,
        // Logical
        AND, ORA, EOR, BIT,
        // Arithmetic
        ADC, SBC, CMP, CPX, CPY,
        // Increments & Decrements
        INC, INX, INY, DEC, DEX, DEY,
        // Jumps & Calls
        JMP, JSR, RTS,
        // Control
        BRK, RTI, NOP,
        // Branches
        BCC, BCS, BEQ, BMI, BNE, BPL, BVC, BVS,
        // Status Flag Changes
        CLC, CLD, CLI, CLV, SEC, SED, SEI,

        // Unofficial Opcodes
        // Load/Store
        LAS, LAX,
        SAX, SHA, SHX, SHY,
        // Transfer
        SHS,
        // Arithmetic
        ANC, ARR, ASR, DCP, ISC, RLA, RRA, SBX, SLO, SRE, XAA,
        // Kill
        JAM,
        // Invalid
        INVALID,
    };

    enum class AddressingMode {
        Implied,
        Accumulator,
        Immediate,
        Absolute,
        X_Indexed_Absolute,
        Y_Indexed_Absolute,
        Zero_Page,
        X_Indexed_Zero_Page,
        Y_Indexed_Zero_Page,
        Indirect,
        X_Indexed_Zero_Page_Indirect,
        Zero_Page_Indirect_Y_Indexed,
        Relative,
        None,
    };

    static constexpr auto AM_IMP = AddressingMode::Implied;
    static constexpr auto AM_ACC = AddressingMode::Accumulator;
    static constexpr auto AM_IMM = AddressingMode::Immediate;
    static constexpr auto AM_ABS = AddressingMode::Absolute;
    static constexpr auto AM_ABS_X = AddressingMode::X_Indexed_Absolute;
    static constexpr auto AM_ABS_Y = AddressingMode::Y_Indexed_Absolute;
    static constexpr auto AM_ZP = AddressingMode::Zero_Page;
    static constexpr auto AM_ZP_X = AddressingMode::X_Indexed_Zero_Page;
    static constexpr auto AM_ZP_Y = AddressingMode::Y_Indexed_Zero_Page;
    static constexpr auto AM_IND = AddressingMode::Indirect;
    static constexpr auto AM_IND_X = AddressingMode::X_Indexed_Zero_Page_Indirect;
    static constexpr auto AM_IND_Y = AddressingMode::Zero_Page_Indirect_Y_Indexed;
    static constexpr auto AM_REL = AddressingMode::Relative;
    static constexpr auto AM_NONE = AddressingMode::None;

    struct OpInfo {
        OpCode op = OpCode::INVALID;
        int cycles = 0;
        AddressingMode mode = AM_NONE;
        bool isStore = false;
    };

    static constexpr OpInfo OP_INFO[256] {
        /* 0x00 */ {OpCode::BRK, 7, AM_IMP},
        /* 0x01 */ {OpCode::ORA, 6, AM_IND_X},
        /* 0x02 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x03 */ {OpCode::SLO, 8, AM_IND_X},
        /* 0x04 */ {OpCode::NOP, 3, AM_ZP},
        /* 0x05 */ {OpCode::ORA, 3, AM_ZP},
        /* 0x06 */ {OpCode::ASL, 5, AM_ZP},
        /* 0x07 */ {OpCode::SLO, 5, AM_ZP},
        /* 0x08 */ {OpCode::PHP, 3, AM_IMP},
        /* 0x09 */ {OpCode::ORA, 2, AM_IMM},
        /* 0x0A */ {OpCode::ASL, 2, AM_ACC},
        /* 0x0B */ {OpCode::ANC, 2, AM_IMM},
        /* 0x0C */ {OpCode::NOP, 4, AM_ABS},
        /* 0x0D */ {OpCode::ORA, 4, AM_ABS},
        /* 0x0E */ {OpCode::ASL, 6, AM_ABS},
        /* 0x0F */ {OpCode::SLO, 6, AM_ABS},
        /* 0x10 */ {OpCode::BPL, 2, AM_REL},
        /* 0x11 */ {OpCode::ORA, 5, AM_IND_Y},
        /* 0x12 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x13 */ {OpCode::SLO, 8, AM_IND_Y, true},
        /* 0x14 */ {OpCode::NOP, 4, AM_ZP_X},
        /* 0x15 */ {OpCode::ORA, 4, AM_ZP_X},
        /* 0x16 */ {OpCode::ASL, 6, AM_ZP_X},
        /* 0x17 */ {OpCode::SLO, 6, AM_ZP_X},
        /* 0x18 */ {OpCode::CLC, 2, AM_IMP},
        /* 0x19 */ {OpCode::ORA, 4, AM_ABS_Y},
        /* 0x1A */ {OpCode::NOP, 2, AM_IMP},
        /* 0x1B */ {OpCode::SLO, 7, AM_ABS_Y, true},
        /* 0x1C */ {OpCode::NOP, 4, AM_ABS_X},
        /* 0x1D */ {OpCode::ORA, 4, AM_ABS_X},
        /* 0x1E */ {OpCode::ASL, 7, AM_ABS_X, true},
        /* 0x1F */ {OpCode::SLO, 7, AM_ABS_X, true},
        /* 0x20 */ {OpCode::JSR, 6, AM_ABS},
        /* 0x21 */ {OpCode::AND, 6, AM_IND_X},
        /* 0x22 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x23 */ {OpCode::RLA, 8, AM_IND_X},
        /* 0x24 */ {OpCode::BIT, 3, AM_ZP},
        /* 0x25 */ {OpCode::AND, 3, AM_ZP},
        /* 0x26 */ {OpCode::ROL, 5, AM_ZP},
        /* 0x27 */ {OpCode::RLA, 5, AM_ZP},
        /* 0x28 */ {OpCode::PLP, 4, AM_IMP},
        /* 0x29 */ {OpCode::AND, 2, AM_IMM},
        /* 0x2A */ {OpCode::ROL, 2, AM_ACC},
        /* 0x2B */ {OpCode::ANC, 2, AM_IMM},
        /* 0x2C */ {OpCode::BIT, 4, AM_ABS},
        /* 0x2D */ {OpCode::AND, 4, AM_ABS},
        /* 0x2E */ {OpCode::ROL, 6, AM_ABS},
        /* 0x2F */ {OpCode::RLA, 6, AM_ABS},
        /* 0x30 */ {OpCode::BMI, 2, AM_REL},
        /* 0x31 */ {OpCode::AND, 5, AM_IND_Y},
        /* 0x32 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x33 */ {OpCode::RLA, 8, AM_IND_Y, true},
        /* 0x34 */ {OpCode::NOP, 4, AM_ZP_X},
        /* 0x35 */ {OpCode::AND, 4, AM_ZP_X},
        /* 0x36 */ {OpCode::ROL, 6, AM_ZP_X},
        /* 0x37 */ {OpCode::RLA, 6, AM_ZP_X},
        /* 0x38 */ {OpCode::SEC, 2, AM_IMP},
        /* 0x39 */ {OpCode::AND, 4, AM_ABS_Y},
        /* 0x3A */ {OpCode::NOP, 2, AM_IMP},
        /* 0x3B */ {OpCode::RLA, 7, AM_ABS_Y, true},
        /* 0x3C */ {OpCode::NOP, 4, AM_ABS_X},
        /* 0x3D */ {OpCode::AND, 4, AM_ABS_X},
        /* 0x3E */ {OpCode::ROL, 7, AM_ABS_X, true},
        /* 0x3F */ {OpCode::RLA, 7, AM_ABS_X, true},
        /* 0x40 */ {OpCode::RTI, 6, AM_IMP},
        /* 0x41 */ {OpCode::EOR, 6, AM_IND_X},
        /* 0x42 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x43 */ {OpCode::SRE, 8, AM_IND_X},
        /* 0x44 */ {OpCode::NOP, 3, AM_ZP},
        /* 0x45 */ {OpCode::EOR, 3, AM_ZP},
        /* 0x46 */ {OpCode::LSR, 5, AM_ZP},
        /* 0x47 */ {OpCode::SRE, 5, AM_ZP},
        /* 0x48 */ {OpCode::PHA, 3, AM_IMP},
        /* 0x49 */ {OpCode::EOR, 2, AM_IMM},
        /* 0x4A */ {OpCode::LSR, 2, AM_ACC},
        /* 0x4B */ {OpCode::ASR, 2, AM_IMM},
        /* 0x4C */ {OpCode::JMP, 3, AM_ABS},
        /* 0x4D */ {OpCode::EOR, 4, AM_ABS},
        /* 0x4E */ {OpCode::LSR, 6, AM_ABS},
        /* 0x4F */ {OpCode::SRE, 6, AM_ABS},
        /* 0x50 */ {OpCode::BVC, 2, AM_REL},
        /* 0x51 */ {OpCode::EOR, 5, AM_IND_Y},
        /* 0x52 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x53 */ {OpCode::SRE, 8, AM_IND_Y, true},
        /* 0x54 */ {OpCode::NOP, 4, AM_ZP_X},
        /* 0x55 */ {OpCode::EOR, 4, AM_ZP_X},
        /* 0x56 */ {OpCode::LSR, 6, AM_ZP_X},
        /* 0x57 */ {OpCode::SRE, 6, AM_ZP_X},
        /* 0x58 */ {OpCode::CLI, 2, AM_IMP},
        /* 0x59 */ {OpCode::EOR, 4, AM_ABS_Y},
        /* 0x5A */ {OpCode::NOP, 2, AM_IMP},
        /* 0x5B */ {OpCode::SRE, 7, AM_ABS_Y, true},
        /* 0x5C */ {OpCode::NOP, 4, AM_ABS_X},
        /* 0x5D */ {OpCode::EOR, 4, AM_ABS_X},
        /* 0x5E */ {OpCode::LSR, 7, AM_ABS_X, true},
        /* 0x5F */ {OpCode::SRE, 7, AM_ABS_X, true},
        /* 0x60 */ {OpCode::RTS, 6, AM_IMP},
        /* 0x61 */ {OpCode::ADC, 6, AM_IND_X},
        /* 0x62 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x63 */ {OpCode::RRA, 8, AM_IND_X},
        /* 0x64 */ {OpCode::NOP, 3, AM_ZP},
        /* 0x65 */ {OpCode::ADC, 3, AM_ZP},
        /* 0x66 */ {OpCode::ROR, 5, AM_ZP},
        /* 0x67 */ {OpCode::RRA, 5, AM_ZP},
        /* 0x68 */ {OpCode::PLA, 4, AM_IMP},
        /* 0x69 */ {OpCode::ADC, 2, AM_IMM},
        /* 0x6A */ {OpCode::ROR, 2, AM_ACC},
        /* 0x6B */ {OpCode::ARR, 2, AM_IMM},
        /* 0x6C */ {OpCode::JMP, 5, AM_IND},
        /* 0x6D */ {OpCode::ADC, 4, AM_ABS},
        /* 0x6E */ {OpCode::ROR, 6, AM_ABS},
        /* 0x6F */ {OpCode::RRA, 6, AM_ABS},
        /* 0x70 */ {OpCode::BVS, 2, AM_REL},
        /* 0x71 */ {OpCode::ADC, 5, AM_IND_Y},
        /* 0x72 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x73 */ {OpCode::RRA, 8, AM_IND_Y, true},
        /* 0x74 */ {OpCode::NOP, 4, AM_ZP_X},
        /* 0x75 */ {OpCode::ADC, 4, AM_ZP_X},
        /* 0x76 */ {OpCode::ROR, 6, AM_ZP_X},
        /* 0x77 */ {OpCode::RRA, 6, AM_ZP_X},
        /* 0x78 */ {OpCode::SEI, 2, AM_IMP},
        /* 0x79 */ {OpCode::ADC, 4, AM_ABS_Y},
        /* 0x7A */ {OpCode::NOP, 2, AM_IMP},
        /* 0x7B */ {OpCode::RRA, 7, AM_ABS_Y, true},
        /* 0x7C */ {OpCode::NOP, 4, AM_ABS_X},
        /* 0x7D */ {OpCode::ADC, 4, AM_ABS_X},
        /* 0x7E */ {OpCode::ROR, 7, AM_ABS_X, true},
        /* 0x7F */ {OpCode::RRA, 7, AM_ABS_X, true},
        /* 0x80 */ {OpCode::NOP, 2, AM_IMM},
        /* 0x81 */ {OpCode::STA, 6, AM_IND_X, true},
        /* 0x82 */ {OpCode::NOP, 2, AM_IMM},
        /* 0x83 */ {OpCode::SAX, 6, AM_IND_X, true},
        /* 0x84 */ {OpCode::STY, 3, AM_ZP, true},
        /* 0x85 */ {OpCode::STA, 3, AM_ZP, true},
        /* 0x86 */ {OpCode::STX, 3, AM_ZP, true},
        /* 0x87 */ {OpCode::SAX, 3, AM_ZP, true},
        /* 0x88 */ {OpCode::DEY, 2, AM_IMP},
        /* 0x89 */ {OpCode::NOP, 2, AM_IMM},
        /* 0x8A */ {OpCode::TXA, 2, AM_IMP},
        /* 0x8B */ {OpCode::XAA, 2, AM_IMM},
        /* 0x8C */ {OpCode::STY, 4, AM_ABS, true},
        /* 0x8D */ {OpCode::STA, 4, AM_ABS, true},
        /* 0x8E */ {OpCode::STX, 4, AM_ABS, true},
        /* 0x8F */ {OpCode::SAX, 4, AM_ABS, true},
        /* 0x90 */ {OpCode::BCC, 2, AM_REL},
        /* 0x91 */ {OpCode::STA, 6, AM_IND_Y, true},
        /* 0x92 */ {OpCode::JAM, 0, AM_IMP},
        /* 0x93 */ {OpCode::SHA, 6, AM_IND_Y, true},
        /* 0x94 */ {OpCode::STY, 4, AM_ZP_X, true},
        /* 0x95 */ {OpCode::STA, 4, AM_ZP_X, true},
        /* 0x96 */ {OpCode::STX, 4, AM_ZP_Y, true},
        /* 0x97 */ {OpCode::SAX, 4, AM_ZP_Y, true},
        /* 0x98 */ {OpCode::TYA, 2, AM_IMP},
        /* 0x99 */ {OpCode::STA, 5, AM_ABS_Y, true},
        /* 0x9A */ {OpCode::TXS, 2, AM_IMP},
        /* 0x9B */ {OpCode::SHS, 5, AM_ABS_Y, true},
        /* 0x9C */ {OpCode::SHY, 5, AM_ABS_X, true},
        /* 0x9D */ {OpCode::STA, 5, AM_ABS_X, true},
        /* 0x9E */ {OpCode::SHX, 5, AM_ABS_Y, true},
        /* 0x9F */ {OpCode::SHA, 5, AM_ABS_Y, true},
        /* 0xA0 */ {OpCode::LDY, 2, AM_IMM},
        /* 0xA1 */ {OpCode::LDA, 6, AM_IND_X},
        /* 0xA2 */ {OpCode::LDX, 2, AM_IMM},
        /* 0xA3 */ {OpCode::LAX, 6, AM_IND_X},
        /* 0xA4 */ {OpCode::LDY, 3, AM_ZP},
        /* 0xA5 */ {OpCode::LDA, 3, AM_ZP},
        /* 0xA6 */ {OpCode::LDX, 3, AM_ZP},
        /* 0xA7 */ {OpCode::LAX, 3, AM_ZP},
        /* 0xA8 */ {OpCode::TAY, 2, AM_IMP},
        /* 0xA9 */ {OpCode::LDA, 2, AM_IMM},
        /* 0xAA */ {OpCode::TAX, 2, AM_IMP},
        /* 0xAB */ {OpCode::LAX, 2, AM_IMM},
        /* 0xAC */ {OpCode::LDY, 4, AM_ABS},
        /* 0xAD */ {OpCode::LDA, 4, AM_ABS},
        /* 0xAE */ {OpCode::LDX, 4, AM_ABS},
        /* 0xAF */ {OpCode::LAX, 4, AM_ABS},
        /* 0xB0 */ {OpCode::BCS, 2, AM_REL},
        /* 0xB1 */ {OpCode::LDA, 5, AM_IND_Y},
        /* 0xB2 */ {OpCode::JAM, 0, AM_IMP},
        /* 0xB3 */ {OpCode::LAX, 5, AM_IND_Y},
        /* 0xB4 */ {OpCode::LDY, 4, AM_ZP_X},
        /* 0xB5 */ {OpCode::LDA, 4, AM_ZP_X},
        /* 0xB6 */ {OpCode::LDX, 4, AM_ZP_Y},
        /* 0xB7 */ {OpCode::LAX, 4, AM_ZP_Y},
        /* 0xB8 */ {OpCode::CLV, 2, AM_IMP},
        /* 0xB9 */ {OpCode::LDA, 4, AM_ABS_Y},
        /* 0xBA */ {OpCode::TSX, 2, AM_IMP},
        /* 0xBB */ {OpCode::LAS, 4, AM_ABS_Y},
        /* 0xBC */ {OpCode::LDY, 4, AM_ABS_X},
        /* 0xBD */ {OpCode::LDA, 4, AM_ABS_X},
        /* 0xBE */ {OpCode::LDX, 4, AM_ABS_Y},
        /* 0xBF */ {OpCode::LAX, 4, AM_ABS_Y},
        /* 0xC0 */ {OpCode::CPY, 2, AM_IMM},
        /* 0xC1 */ {OpCode::CMP, 6, AM_IND_X},
        /* 0xC2 */ {OpCode::NOP, 2, AM_IMM},
        /* 0xC3 */ {OpCode::DCP, 8, AM_IND_X},
        /* 0xC4 */ {OpCode::CPY, 3, AM_ZP},
        /* 0xC5 */ {OpCode::CMP, 3, AM_ZP},
        /* 0xC6 */ {OpCode::DEC, 5, AM_ZP},
        /* 0xC7 */ {OpCode::DCP, 5, AM_ZP},
        /* 0xC8 */ {OpCode::INY, 2, AM_IMP},
        /* 0xC9 */ {OpCode::CMP, 2, AM_IMM},
        /* 0xCA */ {OpCode::DEX, 2, AM_IMP},
        /* 0xCB */ {OpCode::SBX, 2, AM_IMM},
        /* 0xCC */ {OpCode::CPY, 4, AM_ABS},
        /* 0xCD */ {OpCode::CMP, 4, AM_ABS},
        /* 0xCE */ {OpCode::DEC, 6, AM_ABS},
        /* 0xCF */ {OpCode::DCP, 6, AM_ABS},
        /* 0xD0 */ {OpCode::BNE, 2, AM_REL},
        /* 0xD1 */ {OpCode::CMP, 5, AM_IND_Y},
        /* 0xD2 */ {OpCode::JAM, 0, AM_IMP},
        /* 0xD3 */ {OpCode::DCP, 8, AM_IND_Y, true},
        /* 0xD4 */ {OpCode::NOP, 4, AM_ZP_X},
        /* 0xD5 */ {OpCode::CMP, 4, AM_ZP_X},
        /* 0xD6 */ {OpCode::DEC, 6, AM_ZP_X},
        /* 0xD7 */ {OpCode::DCP, 6, AM_ZP_X},
        /* 0xD8 */ {OpCode::CLD, 2, AM_IMP},
        /* 0xD9 */ {OpCode::CMP, 4, AM_ABS_Y},
        /* 0xDA */ {OpCode::NOP, 2, AM_IMP},
        /* 0xDB */ {OpCode::DCP, 7, AM_ABS_Y, true},
        /* 0xDC */ {OpCode::NOP, 4, AM_ABS_X},
        /* 0xDD */ {OpCode::CMP, 4, AM_ABS_X},
        /* 0xDE */ {OpCode::DEC, 7, AM_ABS_X, true},
        /* 0xDF */ {OpCode::DCP, 7, AM_ABS_X, true},
        /* 0xE0 */ {OpCode::CPX, 2, AM_IMM},
        /* 0xE1 */ {OpCode::SBC, 6, AM_IND_X},
        /* 0xE2 */ {OpCode::NOP, 2, AM_IMM},
        /* 0xE3 */ {OpCode::ISC, 8, AM_IND_X},
        /* 0xE4 */ {OpCode::CPX, 3, AM_ZP},
        /* 0xE5 */ {OpCode::SBC, 3, AM_ZP},
        /* 0xE6 */ {OpCode::INC, 5, AM_ZP},
        /* 0xE7 */ {OpCode::ISC, 5, AM_ZP},
        /* 0xE8 */ {OpCode::INX, 2, AM_IMP},
        /* 0xE9 */ {OpCode::SBC, 2, AM_IMM},
        /* 0xEA */ {OpCode::NOP, 2, AM_IMP},
        /* 0xEB */ {OpCode::SBC, 2, AM_IMM},
        /* 0xEC */ {OpCode::CPX, 4, AM_ABS},
        /* 0xED */ {OpCode::SBC, 4, AM_ABS},
        /* 0xEE */ {OpCode::INC, 6, AM_ABS},
        /* 0xEF */ {OpCode::ISC, 6, AM_ABS},
        /* 0xF0 */ {OpCode::BEQ, 2, AM_REL},
        /* 0xF1 */ {OpCode::SBC, 5, AM_IND_Y},
        /* 0xF2 */ {OpCode::JAM, 0, AM_IMP},
        /* 0xF3 */ {OpCode::ISC, 8, AM_IND_Y, true},
        /* 0xF4 */ {OpCode::NOP, 4, AM_ZP_X},
        /* 0xF5 */ {OpCode::SBC, 4, AM_ZP_X},
        /* 0xF6 */ {OpCode::INC, 6, AM_ZP_X},
        /* 0xF7 */ {OpCode::ISC, 6, AM_ZP_X},
        /* 0xF8 */ {OpCode::SED, 2, AM_IMP},
        /* 0xF9 */ {OpCode::SBC, 4, AM_ABS_Y},
        /* 0xFA */ {OpCode::NOP, 2, AM_IMP},
        /* 0xFB */ {OpCode::ISC, 7, AM_ABS_Y, true},
        /* 0xFC */ {OpCode::NOP, 4, AM_ABS_X},
        /* 0xFD */ {OpCode::SBC, 4, AM_ABS_X},
        /* 0xFE */ {OpCode::INC, 7, AM_ABS_X, true},
        /* 0xFF */ {OpCode::ISC, 7, AM_ABS_X, true},
    };

    void CPURegisters::reset() {
        P = DEFAULT_STATE;
        SP = 0xFD;
    }

    void CPURegisters::updateZ(const std::uint8_t val) {
        if (val == 0) {
            setBit(P, INDEX_Z);
        } else {
            clearBit(P, INDEX_Z);
        }
    }

    void CPURegisters::updateN(const std::uint8_t val) {
        if (val & 0b10000000) {
            setBit(P, INDEX_N);
        } else {
            clearBit(P, INDEX_N);
        }
    }

    void CPURegisters::updateNZ(const std::uint8_t val) {
        updateN(val);
        updateZ(val);
    }

    void OperationExecutionState::reset() {
        executingOp = false;
        code = 0;
        step = 0;
        readLo = 0;
        readHi = 0;
        readDummy = 0;
        baseAddr = 0;
        addr = 0;
        hasPageCross = false;
        addressingFinishedStep = 0xFF;
        finished = false;
        isImplied = false;
        isStore = false;
    }

    CPU::CPU(Bus *bus) {
        bus_ = bus;
    }

    void CPU::reset() {
        regs_.reset();
        regs_.PC = (bus_->cpuRead(CPU_VECTOR_RESET + 1) << 8) | bus_->cpuRead(CPU_VECTOR_RESET);
        opState_.reset();
        cycles_ = 0;
        steps_ = 0;
        stallCycles_ = 0;
        halted_ = false;
        nmiPending_ = false;
        nmiActive_ = false;
        irqActive_ = false;
        dmaPending_ = false;
        dmaIndex_ = 0;
        dmaPage_ = 0;
        dmaAlignCycle_ = false;
        prevIRQInhibit_ = true;
    }

    std::uint8_t CPU::cpuRead(const std::uint16_t addr) const {
        return mem_[addr % CPU_RAM_SIZE];
    }

    void CPU::cpuWrite(const std::uint16_t addr, const std::uint16_t value) {
        mem_[addr % CPU_RAM_SIZE] = value;
    }

    void CPU::triggerDMA(const std::uint8_t page) {
        dmaPending_ = true;
        dmaPage_ = page;
        dmaIndex_ = 0;
        dmaAlignCycle_ = true;
    }

    void CPU::step() {
        if (halted_) {
            return;
        }
        if (stallCycles_ > 0) {
            --stallCycles_;
            ++cycles_;
            return;
        }
        if (dmaPending_) {
            if (dmaAlignCycle_) {
                dmaAlignCycle_ = false;
                if (cycles_ % 2 == 1) {
                    ++stallCycles_;
                }
                ++cycles_;
                return;
            }
            const auto addr = (static_cast<std::uint16_t>(dmaPage_) << 8) | dmaIndex_;
            if (cycles_ % 2 == 1) {
                dmaByte_ = bus_->cpuRead(addr);
            } else {
                bus_->cpuWrite(0x2004, dmaByte_);
                ++dmaIndex_;
            }
            if (dmaIndex_ >= 256) {
                dmaPending_ = false;
            }
            ++cycles_;
            return;
        }
        if (opState_.step == 0) {
            prevIRQInhibit_ = regs_.I();
        }
        if (nmiActive_) {
            handleNMI();
        } else if (irqActive_) {
            handleIRQ();
        } else if (nmiLatched_) {
            nmiLatched_ = false;
            nmiActive_ = true;
            ++cycles_;
            return;
        } else if (opState_.step == 0) {
            opState_.code = bus_->cpuRead(regs_.PC++);
            const auto& info = OP_INFO[opState_.code];
            opState_.isStore = info.isStore;
            opState_.executingOp = true;
        } else {
            const auto& info = OP_INFO[opState_.code];
            switch (info.mode) {
                case AM_IMP:
                case AM_ACC:
                    opState_.addressingFinishedStep = 0;
                    opState_.isImplied = true;
                    break;
                case AM_IMM: addrImmediate(); break;
                case AM_ABS:
                    if (info.op == OpCode::JMP) {
                        if (opState_.step == 1) {
                            opState_.readLo = bus_->cpuRead(regs_.PC++);
                        } else if (opState_.step == 2) {
                            opState_.readHi = bus_->cpuRead(regs_.PC++);
                            opState_.addr = (opState_.readHi << 8) | opState_.readLo;
                            opState_.addressingFinishedStep = 1;
                        }
                    } else if (info.op != OpCode::JSR) {
                        addrAbsolute();
                    }
                    break;
                case AM_ABS_X: addrAbsoluteX(); break;
                case AM_ABS_Y: addrAbsoluteY(); break;
                case AM_ZP: addrZeroPage(); break;
                case AM_ZP_X: addrZeroPageX(); break;
                case AM_ZP_Y: addrZeroPageY(); break;
                case AM_IND_X: addrZeroPageIndirectX(); break;
                case AM_IND_Y: addrZeroPageIndirectY(); break;
                case AM_IND: addrAbsoluteIndirect(); break;
                case AM_REL: addrRelative(); break;
                default:
                    throw std::runtime_error("Invalid addressing mode");
            }
            switch (info.op) {
                case OpCode::NOP: execNOP(); break;
                case OpCode::LDA: execLDA(); break;
                case OpCode::LDX: execLDX(); break;
                case OpCode::LDY: execLDY(); break;
                case OpCode::STA: execSTA(); break;
                case OpCode::STX: execSTX(); break;
                case OpCode::STY: execSTY(); break;
                case OpCode::TAX: execTAX(); break;
                case OpCode::TAY: execTAY(); break;
                case OpCode::TXA: execTXA(); break;
                case OpCode::TYA: execTYA(); break;
                case OpCode::TSX: execTSX(); break;
                case OpCode::TXS: execTXS(); break;
                case OpCode::PHA: execPHA(); break;
                case OpCode::PHP: execPHP(); break;
                case OpCode::PLA: execPLA(); break;
                case OpCode::PLP: execPLP(); break;
                case OpCode::ASL: execASL(); break;
                case OpCode::LSR: execLSR(); break;
                case OpCode::ROL: execROL(); break;
                case OpCode::ROR: execROR(); break;
                case OpCode::AND: execAND(); break;
                case OpCode::ORA: execORA(); break;
                case OpCode::EOR: execEOR(); break;
                case OpCode::BIT: execBIT(); break;
                case OpCode::ADC: execADC(); break;
                case OpCode::SBC: execSBC(); break;
                case OpCode::CMP: execCMP(); break;
                case OpCode::CPX: execCPX(); break;
                case OpCode::CPY: execCPY(); break;
                case OpCode::INC: execINC(); break;
                case OpCode::INX: execINX(); break;
                case OpCode::INY: execINY(); break;
                case OpCode::DEC: execDEC(); break;
                case OpCode::DEX: execDEX(); break;
                case OpCode::DEY: execDEY(); break;
                case OpCode::JMP: execJMP(); break;
                case OpCode::JSR: execJSR(); break;
                case OpCode::RTS: execRTS(); break;
                case OpCode::BRK: execBRK(); break;
                case OpCode::RTI: execRTI(); break;
                case OpCode::BCC: execBCC(); break;
                case OpCode::BCS: execBCS(); break;
                case OpCode::BEQ: execBEQ(); break;
                case OpCode::BMI: execBMI(); break;
                case OpCode::BNE: execBNE(); break;
                case OpCode::BPL: execBPL(); break;
                case OpCode::BVC: execBVC(); break;
                case OpCode::BVS: execBVS(); break;
                case OpCode::CLC: execCLC(); break;
                case OpCode::CLD: execCLD(); break;
                case OpCode::CLI: execCLI(); break;
                case OpCode::CLV: execCLV(); break;
                case OpCode::SEC: execSEC(); break;
                case OpCode::SED: execSED(); break;
                case OpCode::SEI: execSEI(); break;
                case OpCode::LAS: execLAS(); break;
                case OpCode::LAX: execLAX(); break;
                case OpCode::SAX: execSAX(); break;
                case OpCode::SHA: execSHA(); break;
                case OpCode::SHX: execSHX(); break;
                case OpCode::SHY: execSHY(); break;
                case OpCode::SHS: execSHS(); break;
                case OpCode::ANC: execANC(); break;
                case OpCode::ARR: execARR(); break;
                case OpCode::ASR: execASR(); break;
                case OpCode::DCP: execDCP(); break;
                case OpCode::ISC: execISC(); break;
                case OpCode::RLA: execRLA(); break;
                case OpCode::RRA: execRRA(); break;
                case OpCode::SBX: execSBX(); break;
                case OpCode::SLO: execSLO(); break;
                case OpCode::SRE: execSRE(); break;
                case OpCode::XAA: execXAA(); break;
                case OpCode::JAM: execJAM(); break;
                default:
                    throw std::runtime_error("Invalid opcode");
            }
        }
        if (opState_.finished) {
            opState_.executingOp = false;
            opState_.reset();
            ++steps_;
            if (nmiPending_) {
                nmiPending_ = false;
                nmiLatched_ = true;
            } else if (bus_->irqAsserted() && !prevIRQInhibit_) {
                irqActive_ = true;
            }
        } else {
            ++opState_.step;
        }
        ++cycles_;
    }
}
