#include "test_cpu_utils.h"

TEST_F(CPUTestBase, LDA_Immediate) {
    const auto cycles = runProgram({0xA9, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00110100);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_Immediate_Zero) {
    const auto cycles = runProgram({0xA9, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_Immediate_Negative) {
    const auto cycles = runProgram({0xA9, 0xFF}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0xFF);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}
