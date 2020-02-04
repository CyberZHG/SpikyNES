#include "test_cpu_utils.h"

// ==================== ASL ====================

TEST_F(CPUTestBase, ASL_Accumulator) {
    // ASL A ; Shift left accumulator
    emulator.cpu().registers().A = 0x25;
    const auto cycles = runProgram({0x0A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().A, 0x4A);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ASL_Accumulator_CarryOut) {
    // ASL A ; Bit 7 shifts into carry
    emulator.cpu().registers().A = 0x81;
    const auto cycles = runProgram({0x0A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x02);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ASL_Accumulator_Zero) {
    // ASL A ; Result is zero
    emulator.cpu().registers().A = 0x80;
    const auto cycles = runProgram({0x0A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ASL_Accumulator_Negative) {
    // ASL A ; Result is negative
    emulator.cpu().registers().A = 0x40;
    const auto cycles = runProgram({0x0A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ASL_ZeroPage) {
    // ASL $10
    emulator.cpu().write(0x0010, 0x25);
    const auto cycles = runProgram({0x06, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x4A);
    EXPECT_FALSE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, ASL_ZeroPage_CarryOut) {
    // ASL $10 ; Bit 7 shifts into carry
    emulator.cpu().write(0x0010, 0x81);
    const auto cycles = runProgram({0x06, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x02);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, ASL_ZeroPageX) {
    // ASL $10,X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x25);
    const auto cycles = runProgram({0x16, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x4A);
}

TEST_F(CPUTestBase, ASL_ZeroPageX_Wrap) {
    // ASL $F0,X ; Zero page wrap
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().write(0x0010, 0x25);
    const auto cycles = runProgram({0x16, 0xF0}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x4A);
}

TEST_F(CPUTestBase, ASL_Absolute) {
    // ASL $0300
    emulator.cpu().write(0x0300, 0x25);
    const auto cycles = runProgram({0x0E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x4A);
}

TEST_F(CPUTestBase, ASL_AbsoluteX) {
    // ASL $0300,X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x25);
    const auto cycles = runProgram({0x1E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x4A);
}

// ==================== LSR ====================

TEST_F(CPUTestBase, LSR_Accumulator) {
    // LSR A ; Shift right accumulator
    emulator.cpu().registers().A = 0x4A;
    const auto cycles = runProgram({0x4A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().A, 0x25);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());  // N always 0 after LSR
}

TEST_F(CPUTestBase, LSR_Accumulator_CarryOut) {
    // LSR A ; Bit 0 shifts into carry
    emulator.cpu().registers().A = 0x03;
    const auto cycles = runProgram({0x4A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x01);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LSR_Accumulator_Zero) {
    // LSR A ; Result is zero
    emulator.cpu().registers().A = 0x01;
    const auto cycles = runProgram({0x4A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LSR_Accumulator_HighBitCleared) {
    // LSR A ; High bit always cleared (N=0)
    emulator.cpu().registers().A = 0xFF;
    const auto cycles = runProgram({0x4A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x7F);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LSR_ZeroPage) {
    // LSR $10
    emulator.cpu().write(0x0010, 0x4A);
    const auto cycles = runProgram({0x46, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x25);
    EXPECT_FALSE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, LSR_ZeroPage_CarryOut) {
    // LSR $10 ; Bit 0 shifts into carry
    emulator.cpu().write(0x0010, 0x03);
    const auto cycles = runProgram({0x46, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x01);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, LSR_ZeroPageX) {
    // LSR $10,X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x4A);
    const auto cycles = runProgram({0x56, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x25);
}

TEST_F(CPUTestBase, LSR_ZeroPageX_Wrap) {
    // LSR $F0,X ; Zero page wrap
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().write(0x0010, 0x4A);
    const auto cycles = runProgram({0x56, 0xF0}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x25);
}

TEST_F(CPUTestBase, LSR_Absolute) {
    // LSR $0300
    emulator.cpu().write(0x0300, 0x4A);
    const auto cycles = runProgram({0x4E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x25);
}

TEST_F(CPUTestBase, LSR_AbsoluteX) {
    // LSR $0300,X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x4A);
    const auto cycles = runProgram({0x5E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x25);
}

// ==================== ROL ====================

TEST_F(CPUTestBase, ROL_Accumulator) {
    // ROL A ; Rotate left, C=0
    emulator.cpu().registers().A = 0x25;
    emulator.cpu().registers().P &= ~0x01;  // Clear C
    const auto cycles = runProgram({0x2A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().A, 0x4A);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ROL_Accumulator_CarryIn) {
    // ROL A ; Rotate left with C=1, C shifts into bit 0
    emulator.cpu().registers().A = 0x25;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0x2A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x4B);
    EXPECT_FALSE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, ROL_Accumulator_CarryOut) {
    // ROL A ; Bit 7 shifts into C
    emulator.cpu().registers().A = 0x81;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x2A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x02);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, ROL_Accumulator_CarryThrough) {
    // ROL A ; C=1 in, bit 7 out to C
    emulator.cpu().registers().A = 0x80;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0x2A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x01);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, ROL_Accumulator_Zero) {
    // ROL A ; Result is zero
    emulator.cpu().registers().A = 0x80;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x2A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, ROL_Accumulator_Negative) {
    // ROL A ; Result is negative
    emulator.cpu().registers().A = 0x40;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x2A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ROL_ZeroPage) {
    // ROL $10
    emulator.cpu().write(0x0010, 0x25);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x26, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x4A);
}

TEST_F(CPUTestBase, ROL_ZeroPageX) {
    // ROL $10,X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x25);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x36, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x4A);
}

TEST_F(CPUTestBase, ROL_Absolute) {
    // ROL $0300
    emulator.cpu().write(0x0300, 0x25);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x2E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x4A);
}

TEST_F(CPUTestBase, ROL_AbsoluteX) {
    // ROL $0300,X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x25);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x3E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x4A);
}

// ==================== ROR ====================

TEST_F(CPUTestBase, ROR_Accumulator) {
    // ROR A ; Rotate right, C=0
    emulator.cpu().registers().A = 0x4A;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x6A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().A, 0x25);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ROR_Accumulator_CarryIn) {
    // ROR A ; Rotate right with C=1, C shifts into bit 7
    emulator.cpu().registers().A = 0x4A;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0x6A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0xA5);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ROR_Accumulator_CarryOut) {
    // ROR A ; Bit 0 shifts into C
    emulator.cpu().registers().A = 0x03;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x6A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x01);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, ROR_Accumulator_CarryThrough) {
    // ROR A ; C=1 in, bit 0 out to C
    emulator.cpu().registers().A = 0x01;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0x6A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ROR_Accumulator_Zero) {
    // ROR A ; Result is zero
    emulator.cpu().registers().A = 0x01;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x6A}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, ROR_ZeroPage) {
    // ROR $10
    emulator.cpu().write(0x0010, 0x4A);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x66, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0010), 0x25);
}

TEST_F(CPUTestBase, ROR_ZeroPageX) {
    // ROR $10,X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x4A);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x76, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().read(0x0015), 0x25);
}

TEST_F(CPUTestBase, ROR_Absolute) {
    // ROR $0300
    emulator.cpu().write(0x0300, 0x4A);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x6E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0300), 0x25);
}

TEST_F(CPUTestBase, ROR_AbsoluteX) {
    // ROR $0300,X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x4A);
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x7E, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().read(0x0310), 0x25);
}
