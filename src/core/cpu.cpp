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

        isHighPrecision_ = false;
        opState_.reset();

        cycles_ = 0;
        steps_ = 0;
        stallCycles_ = 0;
        halted_ = false;
        nmiPending_ = false;
        nmiActive_ = false;
        irqPending_ = false;
        irqActive_ = false;
        dmaPending_ = false;
        dmaIndex_ = 0;
        dmaPage_ = 0;
        dmaAlignCycle_ = false;
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

    int CPU::step() {
        if (halted_) {
            return 0;
        }
        if (stallCycles_ > 0) {
            --stallCycles_;
            ++cycles_;
            return 1;
        }
        if (dmaPending_) {
            if (dmaAlignCycle_) {
                dmaAlignCycle_ = false;
                if (cycles_ % 2 == 1) {
                    cycles_ += 2;
                    return 2;
                }
                cycles_ += 1;
                return 1;
            }
            const auto addr = (static_cast<std::uint16_t>(dmaPage_) << 8) | dmaIndex_;
            const auto value = bus_->cpuRead(addr);
            bus_->cpuWrite(0x2004, value);
            cycles_ += 2;
            if (++dmaIndex_ >= 256) {
                dmaPending_ = false;
            }
            return 2;
        }
        if (nmiPending_ && !isHighPrecision_ && !nmiActive_) {
            nmiPending_ = false;
            nmiActive_ = true;
            isHighPrecision_ = true;
        }
        if (irqPending_ && !regs_.I() && !isHighPrecision_ && !nmiActive_ && !irqActive_) {
            irqPending_ = false;
            irqActive_ = true;
            isHighPrecision_ = true;
        }
        if (isHighPrecision_) {
            if (nmiActive_) {
                handleNMIStep();
            } else if (irqActive_) {
                handleIRQStep();
            } else {
                const auto& info = OP_INFO[opState_.code];
                switch (info.mode) {
                    case AM_IMP:
                    case AM_ACC:
                        opState_.addressingFinishedStep = 0;
                        opState_.isImplied = true;
                        break;
                    case AM_IMM: addrImmediateStep(); break;
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
                            addrAbsoluteStep();
                        }
                        break;
                    case AM_ABS_X: addrAbsoluteXStep(); break;
                    case AM_ABS_Y: addrAbsoluteYStep(); break;
                    case AM_ZP: addrZeroPageStep(); break;
                    case AM_ZP_X: addrZeroPageXStep(); break;
                    case AM_ZP_Y: addrZeroPageYStep(); break;
                    case AM_IND_X: addrZeroPageIndirectXStep(); break;
                    case AM_IND_Y: addrZeroPageIndirectYStep(); break;
                    case AM_IND: addrAbsoluteIndirectStep(); break;
                    case AM_REL: addrRelativeStep(); break;
                    default:
                        throw std::runtime_error("Invalid addressing mode");
                }
                switch (info.op) {
                    case OpCode::NOP: execNOPStep(); break;
                    case OpCode::LDA: execLDAStep(); break;
                    case OpCode::LDX: execLDXStep(); break;
                    case OpCode::LDY: execLDYStep(); break;
                    case OpCode::STA: execSTAStep(); break;
                    case OpCode::STX: execSTXStep(); break;
                    case OpCode::STY: execSTYStep(); break;
                    case OpCode::TAX: execTAXStep(); break;
                    case OpCode::TAY: execTAYStep(); break;
                    case OpCode::TXA: execTXAStep(); break;
                    case OpCode::TYA: execTYAStep(); break;
                    case OpCode::TSX: execTSXStep(); break;
                    case OpCode::TXS: execTXSStep(); break;
                    case OpCode::PHA: execPHAStep(); break;
                    case OpCode::PHP: execPHPStep(); break;
                    case OpCode::PLA: execPLAStep(); break;
                    case OpCode::PLP: execPLPStep(); break;
                    case OpCode::ASL: execASLStep(); break;
                    case OpCode::LSR: execLSRStep(); break;
                    case OpCode::ROL: execROLStep(); break;
                    case OpCode::ROR: execRORStep(); break;
                    case OpCode::AND: execANDStep(); break;
                    case OpCode::ORA: execORAStep(); break;
                    case OpCode::EOR: execEORStep(); break;
                    case OpCode::BIT: execBITStep(); break;
                    case OpCode::ADC: execADCStep(); break;
                    case OpCode::SBC: execSBCStep(); break;
                    case OpCode::CMP: execCMPStep(); break;
                    case OpCode::CPX: execCPXStep(); break;
                    case OpCode::CPY: execCPYStep(); break;
                    case OpCode::INC: execINCStep(); break;
                    case OpCode::INX: execINXStep(); break;
                    case OpCode::INY: execINYStep(); break;
                    case OpCode::DEC: execDECStep(); break;
                    case OpCode::DEX: execDEXStep(); break;
                    case OpCode::DEY: execDEYStep(); break;
                    case OpCode::JMP: execJMPStep(); break;
                    case OpCode::JSR: execJSRStep(); break;
                    case OpCode::RTS: execRTSStep(); break;
                    case OpCode::BRK: execBRKStep(); break;
                    case OpCode::RTI: execRTIStep(); break;
                    case OpCode::BCC: execBCCStep(); break;
                    case OpCode::BCS: execBCSStep(); break;
                    case OpCode::BEQ: execBEQStep(); break;
                    case OpCode::BMI: execBMIStep(); break;
                    case OpCode::BNE: execBNEStep(); break;
                    case OpCode::BPL: execBPLStep(); break;
                    case OpCode::BVC: execBVCStep(); break;
                    case OpCode::BVS: execBVSStep(); break;
                    case OpCode::CLC: execCLCStep(); break;
                    case OpCode::CLD: execCLDStep(); break;
                    case OpCode::CLI: execCLIStep(); break;
                    case OpCode::CLV: execCLVStep(); break;
                    case OpCode::SEC: execSECStep(); break;
                    case OpCode::SED: execSEDStep(); break;
                    case OpCode::SEI: execSEIStep(); break;
                    case OpCode::LAS: execLASStep(); break;
                    case OpCode::LAX: execLAXStep(); break;
                    case OpCode::SAX: execSAXStep(); break;
                    case OpCode::SHA: execSHAStep(); break;
                    case OpCode::SHX: execSHXStep(); break;
                    case OpCode::SHY: execSHYStep(); break;
                    case OpCode::SHS: execSHSStep(); break;
                    case OpCode::ANC: execANCStep(); break;
                    case OpCode::ARR: execARRStep(); break;
                    case OpCode::ASR: execASRStep(); break;
                    case OpCode::DCP: execDCPStep(); break;
                    case OpCode::ISC: execISCStep(); break;
                    case OpCode::RLA: execRLAStep(); break;
                    case OpCode::RRA: execRRAStep(); break;
                    case OpCode::SBX: execSBXStep(); break;
                    case OpCode::SLO: execSLOStep(); break;
                    case OpCode::SRE: execSREStep(); break;
                    case OpCode::XAA: execXAAStep(); break;
                    case OpCode::JAM: execJAMStep(); break;
                    default:
                        throw std::runtime_error("Invalid opcode");
                }
            }
            if (opState_.finished) {
                isHighPrecision_ = false;
                nmiActive_ = false;
                irqActive_ = false;
                opState_.reset();
                ++steps_;
            } else {
                ++opState_.step;
            }
            ++cycles_;
            return 1;
        }
        opState_.code = bus_->cpuRead(regs_.PC++);
        const auto& info = OP_INFO[opState_.code];
        opState_.step = 1;
        opState_.isStore = info.isStore;
        isHighPrecision_ = true;
        ++cycles_;
        return 1;
    }
}
