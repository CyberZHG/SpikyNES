#include "test_cpu_utils.h"

TEST_F(CPUTestBase, LDA_Immediate) {
    // LDA #$42 ; Load $42
    const auto cycles = runProgram({0xA9, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00110100);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_Immediate_Zero) {
    // LDA #$00 ; Load zero
    const auto cycles = runProgram({0xA9, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_Immediate_Negative) {
    // LDA #$FF ; Load negative value
    const auto cycles = runProgram({0xA9, 0xFF}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0xFF);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_Absolute) {
    // LDA $0300 ; Load from address $0300
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xAD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_Absolute_Zero) {
    // LDA $0300 ; Load zero from address $0300
    emulator.cpu().write(0x0300, 0x00);
    const auto cycles = runProgram({0xAD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_Absolute_Negative) {
    // LDA $0300 ; Load negative value from address $0300
    emulator.cpu().write(0x0300, 0x80);
    const auto cycles = runProgram({0xAD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}
