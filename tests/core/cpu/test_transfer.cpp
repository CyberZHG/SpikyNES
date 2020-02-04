#include "test_cpu_utils.h"

// ==================== TAX ====================

TEST_F(CPUTestBase, TAX) {
    // TAX ; Transfer A to X
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0xAA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TAX_Zero) {
    // TAX ; Transfer zero
    emulator.cpu().registers().A = 0x00;
    const auto cycles = runProgram({0xAA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TAX_Negative) {
    // TAX ; Transfer negative value
    emulator.cpu().registers().A = 0x80;
    const auto cycles = runProgram({0xAA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== TAY ====================

TEST_F(CPUTestBase, TAY) {
    // TAY ; Transfer A to Y
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0xA8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TAY_Zero) {
    // TAY ; Transfer zero
    emulator.cpu().registers().A = 0x00;
    const auto cycles = runProgram({0xA8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TAY_Negative) {
    // TAY ; Transfer negative value
    emulator.cpu().registers().A = 0x80;
    const auto cycles = runProgram({0xA8}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== TXA ====================

TEST_F(CPUTestBase, TXA) {
    // TXA ; Transfer X to A
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0x8A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TXA_Zero) {
    // TXA ; Transfer zero
    emulator.cpu().registers().X = 0x00;
    const auto cycles = runProgram({0x8A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TXA_Negative) {
    // TXA ; Transfer negative value
    emulator.cpu().registers().X = 0x80;
    const auto cycles = runProgram({0x8A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== TYA ====================

TEST_F(CPUTestBase, TYA) {
    // TYA ; Transfer Y to A
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0x98}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TYA_Zero) {
    // TYA ; Transfer zero
    emulator.cpu().registers().Y = 0x00;
    const auto cycles = runProgram({0x98}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TYA_Negative) {
    // TYA ; Transfer negative value
    emulator.cpu().registers().Y = 0x80;
    const auto cycles = runProgram({0x98}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== TSX ====================

TEST_F(CPUTestBase, TSX) {
    // TSX ; Transfer SP to X
    emulator.cpu().registers().SP = 0x42;
    const auto cycles = runProgram({0xBA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TSX_Zero) {
    // TSX ; Transfer zero SP
    emulator.cpu().registers().SP = 0x00;
    const auto cycles = runProgram({0xBA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TSX_Negative) {
    // TSX ; Transfer negative SP value
    emulator.cpu().registers().SP = 0x80;
    const auto cycles = runProgram({0xBA}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== TXS ====================

TEST_F(CPUTestBase, TXS) {
    // TXS ; Transfer X to SP (does NOT affect flags)
    emulator.cpu().registers().X = 0x42;
    emulator.cpu().registers().P = 0b00110100;  // N=0, Z=0
    const auto cycles = runProgram({0x9A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TXS_Zero_NoFlagChange) {
    // TXS ; Transfer zero X to SP (does NOT affect flags)
    emulator.cpu().registers().X = 0x00;
    emulator.cpu().registers().P = 0b00110100;
    const auto cycles = runProgram({0x9A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x00);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, TXS_Negative_NoFlagChange) {
    // TXS ; Transfer negative X to SP (does NOT affect flags)
    emulator.cpu().registers().X = 0x80;
    emulator.cpu().registers().P = 0b00110100;
    const auto cycles = runProgram({0x9A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}
