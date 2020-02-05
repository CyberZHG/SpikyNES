#include "test_cpu_utils.h"

// ==================== NOP (Official) ====================

TEST_F(CPUTestBase, NOP_Implied) {
    // NOP ; Official NOP
    const auto cycles = runProgram({0xEA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
}

// ==================== NOP (Unofficial - Implied) ====================

TEST_F(CPUTestBase, NOP_Implied_1A) {
    // NOP ; Unofficial implied NOP
    const auto cycles = runProgram({0x1A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
}

TEST_F(CPUTestBase, NOP_Implied_3A) {
    // NOP ; Unofficial implied NOP
    const auto cycles = runProgram({0x3A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
}

TEST_F(CPUTestBase, NOP_Implied_5A) {
    // NOP ; Unofficial implied NOP
    const auto cycles = runProgram({0x5A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
}

TEST_F(CPUTestBase, NOP_Implied_7A) {
    // NOP ; Unofficial implied NOP
    const auto cycles = runProgram({0x7A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
}

TEST_F(CPUTestBase, NOP_Implied_DA) {
    // NOP ; Unofficial implied NOP
    const auto cycles = runProgram({0xDA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
}

TEST_F(CPUTestBase, NOP_Implied_FA) {
    // NOP ; Unofficial implied NOP
    const auto cycles = runProgram({0xFA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
}

// ==================== NOP (Unofficial - Immediate) ====================

TEST_F(CPUTestBase, NOP_Immediate_80) {
    // NOP #$00 ; Unofficial immediate NOP (DOP/SKB)
    const auto cycles = runProgram({0x80, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_Immediate_82) {
    // NOP #$00 ; Unofficial immediate NOP (DOP/SKB)
    const auto cycles = runProgram({0x82, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_Immediate_89) {
    // NOP #$00 ; Unofficial immediate NOP (DOP/SKB)
    const auto cycles = runProgram({0x89, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_Immediate_C2) {
    // NOP #$00 ; Unofficial immediate NOP (DOP/SKB)
    const auto cycles = runProgram({0xC2, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_Immediate_E2) {
    // NOP #$00 ; Unofficial immediate NOP (DOP/SKB)
    const auto cycles = runProgram({0xE2, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

// ==================== NOP (Unofficial - Zero Page) ====================

TEST_F(CPUTestBase, NOP_ZeroPage_04) {
    // NOP $00 ; Unofficial zero page NOP (DOP)
    const auto cycles = runProgram({0x04, 0x00}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_ZeroPage_44) {
    // NOP $00 ; Unofficial zero page NOP (DOP)
    const auto cycles = runProgram({0x44, 0x00}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_ZeroPage_64) {
    // NOP $00 ; Unofficial zero page NOP (DOP)
    const auto cycles = runProgram({0x64, 0x00}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

// ==================== NOP (Unofficial - Zero Page X) ====================

TEST_F(CPUTestBase, NOP_ZeroPageX_14) {
    // NOP $00,X ; Unofficial zero page X NOP (DOP)
    const auto cycles = runProgram({0x14, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_ZeroPageX_34) {
    // NOP $00,X ; Unofficial zero page X NOP (DOP)
    const auto cycles = runProgram({0x34, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_ZeroPageX_54) {
    // NOP $00,X ; Unofficial zero page X NOP (DOP)
    const auto cycles = runProgram({0x54, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_ZeroPageX_74) {
    // NOP $00,X ; Unofficial zero page X NOP (DOP)
    const auto cycles = runProgram({0x74, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_ZeroPageX_D4) {
    // NOP $00,X ; Unofficial zero page X NOP (DOP)
    const auto cycles = runProgram({0xD4, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, NOP_ZeroPageX_F4) {
    // NOP $00,X ; Unofficial zero page X NOP (DOP)
    const auto cycles = runProgram({0xF4, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

// ==================== NOP (Unofficial - Absolute) ====================

TEST_F(CPUTestBase, NOP_Absolute_0C) {
    // NOP $0000 ; Unofficial absolute NOP (TOP)
    const auto cycles = runProgram({0x0C, 0x00, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}

// ==================== NOP (Unofficial - Absolute X) ====================

TEST_F(CPUTestBase, NOP_AbsoluteX_1C) {
    // NOP $0000,X ; Unofficial absolute X NOP (TOP)
    const auto cycles = runProgram({0x1C, 0x00, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}

TEST_F(CPUTestBase, NOP_AbsoluteX_3C) {
    // NOP $0000,X ; Unofficial absolute X NOP (TOP)
    const auto cycles = runProgram({0x3C, 0x00, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}

TEST_F(CPUTestBase, NOP_AbsoluteX_5C) {
    // NOP $0000,X ; Unofficial absolute X NOP (TOP)
    const auto cycles = runProgram({0x5C, 0x00, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}

TEST_F(CPUTestBase, NOP_AbsoluteX_7C) {
    // NOP $0000,X ; Unofficial absolute X NOP (TOP)
    const auto cycles = runProgram({0x7C, 0x00, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}

TEST_F(CPUTestBase, NOP_AbsoluteX_DC) {
    // NOP $0000,X ; Unofficial absolute X NOP (TOP)
    const auto cycles = runProgram({0xDC, 0x00, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}

TEST_F(CPUTestBase, NOP_AbsoluteX_FC) {
    // NOP $0000,X ; Unofficial absolute X NOP (TOP)
    const auto cycles = runProgram({0xFC, 0x00, 0x00}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}

TEST_F(CPUTestBase, NOP_AbsoluteX_PageCross) {
    // NOP $00FF,X ; Page cross adds 1 cycle
    emulator.cpu().registers().X = 0x01;
    const auto cycles = runProgram({0x1C, 0xFF, 0x00}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
}
