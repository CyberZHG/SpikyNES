#include "test_cpu_utils.h"

// ==================== PHA ====================

TEST_F(CPUTestBase, PHA) {
    // PHA ; Push A onto stack
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x48}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFE);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), 0x42);
}

TEST_F(CPUTestBase, PHA_NoFlagChange) {
    // PHA ; Should not affect flags
    emulator.cpu().registers().A = 0x00;
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00110100;
    const auto cycles = runProgram({0x48}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), 0x00);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, PHA_StackWrap) {
    // PHA ; Stack wraps from $00 to $FF
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().SP = 0x00;
    const auto cycles = runProgram({0x48}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
    EXPECT_EQ(emulator.cpu().cpuRead(0x0100), 0x42);
}

// ==================== PHP ====================

TEST_F(CPUTestBase, PHP) {
    // PHP ; Push P onto stack with B and bit5 set
    emulator.cpu().registers().P = 0b00100100;
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x08}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFE);
    // Pushed value should have B (bit4) and bit5 set: 0b00110100
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), 0b00110100);
}

TEST_F(CPUTestBase, PHP_SetsBAndBit5) {
    // PHP ; Even if P has B=0 and bit5=0, pushed value has both set
    emulator.cpu().registers().P = 0b00000100;
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x08}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), 0b00110100);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00000100);
}

TEST_F(CPUTestBase, PHP_WithFlags) {
    // PHP ; Push P with N and Z flags set
    emulator.cpu().registers().P = 0b10110110;
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x08}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), 0b10110110);
}

TEST_F(CPUTestBase, PHP_NoFlagChange) {
    // PHP ; Should not affect flags
    emulator.cpu().registers().P = 0b00110100;
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x08}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00110100);
}

// ==================== PLA ====================

TEST_F(CPUTestBase, PLA) {
    // PLA ; Pull A from stack
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().cpuWrite(0x01FF, 0x42);
    const auto cycles = runProgram({0x68}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, PLA_Zero) {
    // PLA ; Pull zero, Z flag set
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().cpuWrite(0x01FF, 0x00);
    const auto cycles = runProgram({0x68}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, PLA_Negative) {
    // PLA ; Pull negative value, N flag set
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().cpuWrite(0x01FF, 0x80);
    const auto cycles = runProgram({0x68}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, PLA_StackWrap) {
    // PLA ; Stack wraps from $FF to $00
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().cpuWrite(0x0100, 0x42);
    const auto cycles = runProgram({0x68}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x00);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
}

// ==================== PLP ====================

TEST_F(CPUTestBase, PLP) {
    // PLP ; Pull P from stack
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().registers().P = 0b00110100;
    emulator.cpu().cpuWrite(0x01FF, 0b11000011);
    const auto cycles = runProgram({0x28}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
    // Bit 5 always set, bit 4 (B) ignored from stack
    EXPECT_EQ(emulator.cpu().registers().P & 0b11001111, 0b11000011 & 0b11001111);
}

TEST_F(CPUTestBase, PLP_ClearsFlags) {
    // PLP ; Pull P that clears flags
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().registers().P = 0b10110110;
    emulator.cpu().cpuWrite(0x01FF, 0b00110100);
    const auto cycles = runProgram({0x28}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(emulator.cpu().registers().N());
    EXPECT_FALSE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, PLP_SetsFlags) {
    // PLP ; Pull P that sets flags
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().registers().P = 0b00110100;
    emulator.cpu().cpuWrite(0x01FF, 0b10110110);
    const auto cycles = runProgram({0x28}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().N());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, PLP_IgnoresBFlag) {
    // PLP ; B flag (bit 4) should be ignored when pulling
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().registers().P = 0b00100100;
    emulator.cpu().cpuWrite(0x01FF, 0b00110100);
    const auto cycles = runProgram({0x28}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_FALSE(emulator.cpu().registers().B());
}

TEST_F(CPUTestBase, PLP_SetsBit5ClearsBit4) {
    // PLP ; Bit 5 always set, bit 4 (B) always cleared
    emulator.cpu().registers().SP = 0xFE;
    emulator.cpu().registers().P = 0b00100100;
    emulator.cpu().cpuWrite(0x01FF, 0b00010100);
    const auto cycles = runProgram({0x28}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00100100);
}

// ==================== PHA + PLA ====================

TEST_F(CPUTestBase, PHA_PLA_RoundTrip) {
    // PHA then PLA ; Value should be preserved
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x48, 0xA9, 0x00, 0x68}, 3);
    // PHA(3) + LDA(2) + PLA(4) = 9 cycles
    EXPECT_EQ(cycles, 9);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
}

// ==================== PHP + PLP ====================

TEST_F(CPUTestBase, PHP_PLP_RoundTrip) {
    // PHP then PLP ; Flags should be preserved (except B)
    emulator.cpu().registers().P = 0b11110111;
    emulator.cpu().registers().SP = 0xFF;
    const auto cycles = runProgram({0x08, 0x28}, 2);
    // PHP(3) + PLP(4) = 7 cycles
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
    EXPECT_TRUE(emulator.cpu().registers().N());
    EXPECT_TRUE(emulator.cpu().registers().V());
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().C());
}
