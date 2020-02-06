#include "test_cpu_utils.h"

// ==================== STA ====================

TEST_F(CPUTestBase, STA_Absolute) {
    // STA $0300 ; Store A to address $0300
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0x8D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STA_Absolute_NoFlagChange) {
    // STA $0300 ; Store A to address $0300
    emulator.cpu().registers().A = 0x00;
    emulator.cpu().registers().P = 0b00110100;
    const auto cycles = runProgram({0x8D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x00);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, STA_Absolute_NoFlagChange_Negative) {
    // STA $0300 ; Store A to address $0300
    emulator.cpu().registers().A = 0x80;
    emulator.cpu().registers().P = 0b00110100;
    const auto cycles = runProgram({0x8D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, STA_AbsoluteX) {
    // STA $0300,X
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x10;
    const auto cycles = runProgram({0x9D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x42);
}

TEST_F(CPUTestBase, STA_AbsoluteX_PageCross_NoPenalty) {
    // STA $0385,X with X=$80 crosses page, but stores have no page cross penalty
    emulator.cpu().registers().A = 0x99;
    emulator.cpu().registers().X = 0x80;
    const auto cycles = runProgram({0x9D, 0x85, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0405), 0x99);
}

TEST_F(CPUTestBase, STA_AbsoluteY) {
    // STA $0300,Y
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().Y = 0x10;
    const auto cycles = runProgram({0x99, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x42);
}

TEST_F(CPUTestBase, STA_ZeroPage) {
    // STA $10
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0x85, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STA_ZeroPageX) {
    // STA $10,X
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x05;
    const auto cycles = runProgram({0x95, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x42);
}

TEST_F(CPUTestBase, STA_ZeroPageX_Wrap) {
    // STA $F0,X ; Zero page wrap
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x20;
    const auto cycles = runProgram({0x95, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STA_IndirectX) {
    // STA ($10,X)
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    const auto cycles = runProgram({0x81, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STA_IndirectY) {
    // STA ($10),Y
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    const auto cycles = runProgram({0x91, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x42);
}

// ==================== STX ====================

TEST_F(CPUTestBase, STX_Absolute) {
    // STX $0300
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0x8E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STX_ZeroPage) {
    // STX $10
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0x86, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STX_ZeroPageY) {
    // STX $10,Y
    emulator.cpu().registers().X = 0x42;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x96, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x42);
}

TEST_F(CPUTestBase, STX_ZeroPageY_Wrap) {
    // STX $F0,Y ; Zero page wrap
    emulator.cpu().registers().X = 0x42;
    emulator.cpu().registers().Y = 0x20;
    const auto cycles = runProgram({0x96, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

// ==================== STY ====================

TEST_F(CPUTestBase, STY_Absolute) {
    // STY $0300
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0x8C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x42);
}

TEST_F(CPUTestBase, STY_ZeroPage) {
    // STY $10
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0x84, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

TEST_F(CPUTestBase, STY_ZeroPageX) {
    // STY $10,X
    emulator.cpu().registers().Y = 0x42;
    emulator.cpu().registers().X = 0x05;
    const auto cycles = runProgram({0x94, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x42);
}

TEST_F(CPUTestBase, STY_ZeroPageX_Wrap) {
    // STY $F0,X ; Zero page wrap
    emulator.cpu().registers().Y = 0x42;
    emulator.cpu().registers().X = 0x20;
    const auto cycles = runProgram({0x94, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x42);
}

// ==================== SAX (Unofficial) ====================

TEST_F(CPUTestBase, SAX_ZeroPage) {
    // SAX $10 ; Store A AND X to zero page
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().registers().X = 0x0F;
    const auto cycles = runProgram({0x87, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x0F);
}

TEST_F(CPUTestBase, SAX_ZeroPageY) {
    // SAX $10,Y ; Store A AND X to zero page + Y
    emulator.cpu().registers().A = 0xF0;
    emulator.cpu().registers().X = 0x3C;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x97, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x30);
}

TEST_F(CPUTestBase, SAX_Absolute) {
    // SAX $0300 ; Store A AND X to absolute address
    emulator.cpu().registers().A = 0xAA;
    emulator.cpu().registers().X = 0x55;
    const auto cycles = runProgram({0x8F, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x00);
}

TEST_F(CPUTestBase, SAX_IndirectX) {
    // SAX ($10,X) ; Store A AND X via indirect X
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    const auto cycles = runProgram({0x83, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x05);
}

// ==================== SHA (Unofficial) ====================

TEST_F(CPUTestBase, SHA_AbsoluteY) {
    // SHA $0300,Y ; Store A AND X AND (high byte + 1)
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().registers().X = 0xFF;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x9F, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}

TEST_F(CPUTestBase, SHA_IndirectY) {
    // SHA ($10),Y ; Store A AND X AND (high byte + 1)
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().registers().X = 0xFF;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    const auto cycles = runProgram({0x93, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}

// ==================== SHX (Unofficial) ====================

TEST_F(CPUTestBase, SHX_AbsoluteY) {
    // SHX $0300,Y ; Store X AND (high byte + 1)
    emulator.cpu().registers().X = 0xFF;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x9E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}

TEST_F(CPUTestBase, SHX_AbsoluteY_Mask) {
    // SHX $0300,Y ; X is masked by (high byte + 1)
    emulator.cpu().registers().X = 0x0F;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x9E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}

// ==================== SHY (Unofficial) ====================

TEST_F(CPUTestBase, SHY_AbsoluteX) {
    // SHY $0300,X ; Store Y AND (high byte + 1)
    emulator.cpu().registers().Y = 0xFF;
    emulator.cpu().registers().X = 0x05;
    const auto cycles = runProgram({0x9C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}

TEST_F(CPUTestBase, SHY_AbsoluteX_Mask) {
    // SHY $0300,X ; Y is masked by (high byte + 1)
    emulator.cpu().registers().Y = 0x0F;
    emulator.cpu().registers().X = 0x05;
    const auto cycles = runProgram({0x9C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}

// ==================== SHS (Unofficial) ====================

TEST_F(CPUTestBase, SHS_AbsoluteY) {
    // SHS $0300,Y ; SP = A AND X, store SP AND (high byte + 1)
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().registers().X = 0xFF;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x9B, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}

TEST_F(CPUTestBase, SHS_AbsoluteY_SPSet) {
    // SHS $0300,Y ; SP becomes A AND X
    emulator.cpu().registers().A = 0xF0;
    emulator.cpu().registers().X = 0x0F;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x9B, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x00);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x00);
}

TEST_F(CPUTestBase, SHS_AbsoluteY_Masked) {
    // SHS $0300,Y ; Result masked by (high byte + 1)
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().registers().X = 0x0F;
    emulator.cpu().registers().Y = 0x05;
    const auto cycles = runProgram({0x9B, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x0F);
    EXPECT_EQ(emulator.cpu().read(0x0305), 0x04);
}
