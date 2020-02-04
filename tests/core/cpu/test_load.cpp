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

TEST_F(CPUTestBase, LDA_AbsoluteX) {
    // LDA $0300,X ; Load from address $0300 + X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x42);
    const auto cycles = runProgram({0xBD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_AbsoluteX_PageCross) {
    // LDA $03F0,X ; Load with page crossing ($03F0 + $20 = $0410)
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().write(0x0410, 0x42);
    const auto cycles = runProgram({0xBD, 0xF0, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_AbsoluteX_Zero) {
    // LDA $0300,X ; Load zero
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x00);
    const auto cycles = runProgram({0xBD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_AbsoluteX_Negative) {
    // LDA $0300,X ; Load negative value
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x80);
    const auto cycles = runProgram({0xBD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_AbsoluteY) {
    // LDA $0300,Y ; Load from address $0300 + Y
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0x42);
    const auto cycles = runProgram({0xB9, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_AbsoluteY_PageCross) {
    // LDA $03F0,Y ; Load with page crossing ($03F0 + $20 = $0410)
    emulator.cpu().registers().Y = 0x20;
    emulator.cpu().write(0x0410, 0x42);
    const auto cycles = runProgram({0xB9, 0xF0, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_AbsoluteY_Zero) {
    // LDA $0300,Y ; Load zero
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0x00);
    const auto cycles = runProgram({0xB9, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_AbsoluteY_Negative) {
    // LDA $0300,Y ; Load negative value
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0x80);
    const auto cycles = runProgram({0xB9, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_ZeroPage) {
    // LDA $10 ; Load from zero page address $10
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xA5, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_ZeroPage_Zero) {
    // LDA $10 ; Load zero from zero page
    emulator.cpu().write(0x0010, 0x00);
    const auto cycles = runProgram({0xA5, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_ZeroPage_Negative) {
    // LDA $10 ; Load negative value from zero page
    emulator.cpu().write(0x0010, 0x80);
    const auto cycles = runProgram({0xA5, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_ZeroPageX) {
    // LDA $10,X ; Load from zero page address $10 + X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x42);
    const auto cycles = runProgram({0xB5, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_ZeroPageX_Wrap) {
    // LDA $F0,X ; Load with zero page wrap ($F0 + $20 = $10, wraps in zero page)
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xB5, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_ZeroPageX_Zero) {
    // LDA $10,X ; Load zero
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    const auto cycles = runProgram({0xB5, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_ZeroPageX_Negative) {
    // LDA $10,X ; Load negative value
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x80);
    const auto cycles = runProgram({0xB5, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_IndirectX) {
    // LDA ($10,X) ; Load from address stored at ($10 + X)
    // X = $05, read pointer from $15/$16, pointer = $0300, load from $0300
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xA1, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_IndirectX_Wrap) {
    // LDA ($F0,X) ; Zero page wrap: $F0 + $20 = $110 -> wraps to $10
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xA1, 0xF0}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_IndirectX_PointerWrap) {
    // LDA ($10,X) ; Pointer at $FF/$00 (wraps within zero page for high byte)
    emulator.cpu().registers().X = 0xEF;
    emulator.cpu().write(0x00FF, 0x00);
    emulator.cpu().write(0x0000, 0x03);
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xA1, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
}

TEST_F(CPUTestBase, LDA_IndirectX_Zero) {
    // LDA ($10,X) ; Load zero
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x00);
    const auto cycles = runProgram({0xA1, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_IndirectX_Negative) {
    // LDA ($10,X) ; Load negative value
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x80);
    const auto cycles = runProgram({0xA1, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}
