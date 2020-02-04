#include "test_cpu_utils.h"

// ==================== AND ====================

TEST_F(CPUTestBase, AND_Immediate) {
    // AND #$0F
    emulator.cpu().registers().A = 0x3C;
    const auto cycles = runProgram({0x29, 0x0F}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, AND_Immediate_Zero) {
    // AND #$00 ; Result is zero
    emulator.cpu().registers().A = 0xFF;
    const auto cycles = runProgram({0x29, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, AND_Immediate_Negative) {
    // AND #$FF ; Result is negative
    emulator.cpu().registers().A = 0x80;
    const auto cycles = runProgram({0x29, 0xFF}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, AND_ZeroPage) {
    // AND $10
    emulator.cpu().registers().A = 0x3C;
    emulator.cpu().write(0x0010, 0x0F);
    const auto cycles = runProgram({0x25, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
}

TEST_F(CPUTestBase, AND_ZeroPageX) {
    // AND $10,X
    emulator.cpu().registers().A = 0x3C;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x0F);
    const auto cycles = runProgram({0x35, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
}

TEST_F(CPUTestBase, AND_Absolute) {
    // AND $0300
    emulator.cpu().registers().A = 0x3C;
    emulator.cpu().write(0x0300, 0x0F);
    const auto cycles = runProgram({0x2D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
}

TEST_F(CPUTestBase, AND_AbsoluteX) {
    // AND $0300,X
    emulator.cpu().registers().A = 0x3C;
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x0F);
    const auto cycles = runProgram({0x3D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
}

TEST_F(CPUTestBase, AND_AbsoluteY) {
    // AND $0300,Y
    emulator.cpu().registers().A = 0x3C;
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0x0F);
    const auto cycles = runProgram({0x39, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
}

TEST_F(CPUTestBase, AND_IndirectX) {
    // AND ($10,X)
    emulator.cpu().registers().A = 0x3C;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x0F);
    const auto cycles = runProgram({0x21, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
}

TEST_F(CPUTestBase, AND_IndirectY) {
    // AND ($10),Y
    emulator.cpu().registers().A = 0x3C;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x0F);
    const auto cycles = runProgram({0x31, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0C);
}

// ==================== ORA ====================

TEST_F(CPUTestBase, ORA_Immediate) {
    // ORA #$0F
    emulator.cpu().registers().A = 0x30;
    const auto cycles = runProgram({0x09, 0x0F}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ORA_Immediate_Zero) {
    // ORA #$00 ; A=0, result is zero
    emulator.cpu().registers().A = 0x00;
    const auto cycles = runProgram({0x09, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ORA_Immediate_Negative) {
    // ORA #$80 ; Result is negative
    emulator.cpu().registers().A = 0x00;
    const auto cycles = runProgram({0x09, 0x80}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ORA_ZeroPage) {
    // ORA $10
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().write(0x0010, 0x0F);
    const auto cycles = runProgram({0x05, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
}

TEST_F(CPUTestBase, ORA_ZeroPageX) {
    // ORA $10,X
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x0F);
    const auto cycles = runProgram({0x15, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
}

TEST_F(CPUTestBase, ORA_Absolute) {
    // ORA $0300
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().write(0x0300, 0x0F);
    const auto cycles = runProgram({0x0D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
}

TEST_F(CPUTestBase, ORA_AbsoluteX) {
    // ORA $0300,X
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x0F);
    const auto cycles = runProgram({0x1D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
}

TEST_F(CPUTestBase, ORA_AbsoluteY) {
    // ORA $0300,Y
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0x0F);
    const auto cycles = runProgram({0x19, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
}

TEST_F(CPUTestBase, ORA_IndirectX) {
    // ORA ($10,X)
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x0F);
    const auto cycles = runProgram({0x01, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
}

TEST_F(CPUTestBase, ORA_IndirectY) {
    // ORA ($10),Y
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x0F);
    const auto cycles = runProgram({0x11, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0x3F);
}

// ==================== EOR ====================

TEST_F(CPUTestBase, EOR_Immediate) {
    // EOR #$FF
    emulator.cpu().registers().A = 0x55;
    const auto cycles = runProgram({0x49, 0xFF}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, EOR_Immediate_Zero) {
    // EOR #$55 ; A=0x55, result is zero
    emulator.cpu().registers().A = 0x55;
    const auto cycles = runProgram({0x49, 0x55}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, EOR_Immediate_NotNegative) {
    // EOR #$80 ; A=0x80, result is positive
    emulator.cpu().registers().A = 0x80;
    const auto cycles = runProgram({0x49, 0x80}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, EOR_ZeroPage) {
    // EOR $10
    emulator.cpu().registers().A = 0x55;
    emulator.cpu().write(0x0010, 0xFF);
    const auto cycles = runProgram({0x45, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
}

TEST_F(CPUTestBase, EOR_ZeroPageX) {
    // EOR $10,X
    emulator.cpu().registers().A = 0x55;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0xFF);
    const auto cycles = runProgram({0x55, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
}

TEST_F(CPUTestBase, EOR_Absolute) {
    // EOR $0300
    emulator.cpu().registers().A = 0x55;
    emulator.cpu().write(0x0300, 0xFF);
    const auto cycles = runProgram({0x4D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
}

TEST_F(CPUTestBase, EOR_AbsoluteX) {
    // EOR $0300,X
    emulator.cpu().registers().A = 0x55;
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0xFF);
    const auto cycles = runProgram({0x5D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
}

TEST_F(CPUTestBase, EOR_AbsoluteY) {
    // EOR $0300,Y
    emulator.cpu().registers().A = 0x55;
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0xFF);
    const auto cycles = runProgram({0x59, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
}

TEST_F(CPUTestBase, EOR_IndirectX) {
    // EOR ($10,X)
    emulator.cpu().registers().A = 0x55;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0xFF);
    const auto cycles = runProgram({0x41, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
}

TEST_F(CPUTestBase, EOR_IndirectY) {
    // EOR ($10),Y
    emulator.cpu().registers().A = 0x55;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0xFF);
    const auto cycles = runProgram({0x51, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0xAA);
}

// ==================== BIT ====================

TEST_F(CPUTestBase, BIT_ZeroPage) {
    // BIT $10 ; Test bits, A & M != 0
    emulator.cpu().registers().A = 0x0F;
    emulator.cpu().write(0x0010, 0x0F);
    const auto cycles = runProgram({0x24, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0F);  // A unchanged
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
    EXPECT_FALSE(emulator.cpu().registers().V());
}

TEST_F(CPUTestBase, BIT_ZeroPage_Zero) {
    // BIT $10 ; A & M == 0, Z flag set
    emulator.cpu().registers().A = 0x0F;
    emulator.cpu().write(0x0010, 0xF0);
    const auto cycles = runProgram({0x24, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0F);  // A unchanged
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());   // bit 7 of M
    EXPECT_TRUE(emulator.cpu().registers().V());   // bit 6 of M
}

TEST_F(CPUTestBase, BIT_ZeroPage_NFlag) {
    // BIT $10 ; N = bit 7 of memory
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().write(0x0010, 0x80);
    const auto cycles = runProgram({0x24, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_TRUE(emulator.cpu().registers().N());
    EXPECT_FALSE(emulator.cpu().registers().V());
}

TEST_F(CPUTestBase, BIT_ZeroPage_VFlag) {
    // BIT $10 ; V = bit 6 of memory
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().write(0x0010, 0x40);
    const auto cycles = runProgram({0x24, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_FALSE(emulator.cpu().registers().N());
    EXPECT_TRUE(emulator.cpu().registers().V());
}

TEST_F(CPUTestBase, BIT_ZeroPage_AllFlags) {
    // BIT $10 ; N=1, V=1 from memory, Z=1 because A & M == 0
    emulator.cpu().registers().A = 0x00;
    emulator.cpu().write(0x0010, 0xC0);
    const auto cycles = runProgram({0x24, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
    EXPECT_TRUE(emulator.cpu().registers().V());
}

TEST_F(CPUTestBase, BIT_Absolute) {
    // BIT $0300
    emulator.cpu().registers().A = 0x0F;
    emulator.cpu().write(0x0300, 0x0F);
    const auto cycles = runProgram({0x2C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0F);  // A unchanged
    EXPECT_FALSE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, BIT_Absolute_AllFlags) {
    // BIT $0300 ; N=1, V=1 from memory
    emulator.cpu().registers().A = 0x00;
    emulator.cpu().write(0x0300, 0xC0);
    const auto cycles = runProgram({0x2C, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
    EXPECT_TRUE(emulator.cpu().registers().V());
}
