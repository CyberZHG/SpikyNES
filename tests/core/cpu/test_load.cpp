#include "test_cpu_utils.h"

// ==================== LDA ====================

TEST_F(CPUTestBase, LDA_Immediate) {
    // LDA #$42 ; Load $42
    const auto cycles = runProgram({0xA9, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().P, 0b00100100);
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

TEST_F(CPUTestBase, LDA_IndirectY) {
    // LDA ($10),Y ; Read pointer from $10/$11, add Y, load from result
    // Pointer = $0300, Y = $05, load from $0305
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x42);
    const auto cycles = runProgram({0xB1, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_IndirectY_PageCross) {
    // LDA ($10),Y ; Page crossing: $03F0 + $20 = $0410
    emulator.cpu().registers().Y = 0x20;
    emulator.cpu().write(0x0010, 0xF0);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0410, 0x42);
    const auto cycles = runProgram({0xB1, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_IndirectY_PointerWrap) {
    // LDA ($FF),Y ; Pointer at $FF/$00
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x00FF, 0x00);
    emulator.cpu().write(0x0000, 0x03);
    emulator.cpu().write(0x0305, 0x42);
    const auto cycles = runProgram({0xB1, 0xFF}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
}

TEST_F(CPUTestBase, LDA_IndirectY_Zero) {
    // LDA ($10),Y ; Load zero
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x00);
    const auto cycles = runProgram({0xB1, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDA_IndirectY_Negative) {
    // LDA ($10),Y ; Load negative value
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x80);
    const auto cycles = runProgram({0xB1, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

// ==================== LDX ====================

TEST_F(CPUTestBase, LDX_Immediate) {
    // LDX #$42
    const auto cycles = runProgram({0xA2, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDX_Immediate_Zero) {
    // LDX #$00
    const auto cycles = runProgram({0xA2, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDX_Immediate_Negative) {
    // LDX #$80
    const auto cycles = runProgram({0xA2, 0x80}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDX_Absolute) {
    // LDX $0300
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xAE, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LDX_AbsoluteY) {
    // LDX $0300,Y
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0x42);
    const auto cycles = runProgram({0xBE, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LDX_AbsoluteY_PageCross) {
    // LDX $03F0,Y ; Page crossing: $03F0 + $20 = $0410
    emulator.cpu().registers().Y = 0x20;
    emulator.cpu().write(0x0410, 0x42);
    const auto cycles = runProgram({0xBE, 0xF0, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LDX_ZeroPage) {
    // LDX $10
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xA6, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LDX_ZeroPageY) {
    // LDX $10,Y
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0015, 0x42);
    const auto cycles = runProgram({0xB6, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LDX_ZeroPageY_Wrap) {
    // LDX $F0,Y ; Zero page wrap: $F0 + $20 = $110 -> wraps to $10
    emulator.cpu().registers().Y = 0x20;
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xB6, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

// ==================== LDY ====================

TEST_F(CPUTestBase, LDY_Immediate) {
    // LDY #$42
    const auto cycles = runProgram({0xA0, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDY_Immediate_Zero) {
    // LDY #$00
    const auto cycles = runProgram({0xA0, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDY_Immediate_Negative) {
    // LDY #$80
    const auto cycles = runProgram({0xA0, 0x80}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LDY_Absolute) {
    // LDY $0300
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xAC, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
}

TEST_F(CPUTestBase, LDY_AbsoluteX) {
    // LDY $0300,X
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x42);
    const auto cycles = runProgram({0xBC, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
}

TEST_F(CPUTestBase, LDY_AbsoluteX_PageCross) {
    // LDY $03F0,X ; Page crossing: $03F0 + $20 = $0410
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().write(0x0410, 0x42);
    const auto cycles = runProgram({0xBC, 0xF0, 0x03}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
}

TEST_F(CPUTestBase, LDY_ZeroPage) {
    // LDY $10
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xA4, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
}

TEST_F(CPUTestBase, LDY_ZeroPageX) {
    // LDY $10,X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x42);
    const auto cycles = runProgram({0xB4, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
}

TEST_F(CPUTestBase, LDY_ZeroPageX_Wrap) {
    // LDY $F0,X ; Zero page wrap: $F0 + $20 = $110 -> wraps to $10
    emulator.cpu().registers().X = 0x20;
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xB4, 0xF0}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
}

// ==================== LAX (Unofficial) ====================

TEST_F(CPUTestBase, LAX_Immediate) {
    // LAX #$42 ; Load A and X with $42
    const auto cycles = runProgram({0xAB, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LAX_Immediate_Zero) {
    // LAX #$00 ; Load A and X with zero
    const auto cycles = runProgram({0xAB, 0x00}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_EQ(emulator.cpu().registers().X, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LAX_Immediate_Negative) {
    // LAX #$80 ; Load A and X with negative value
    const auto cycles = runProgram({0xAB, 0x80}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_EQ(emulator.cpu().registers().X, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LAX_ZeroPage) {
    // LAX $10 ; Load A and X from zero page
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xA7, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LAX_ZeroPageY) {
    // LAX $10,Y ; Load A and X from zero page + Y
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0015, 0x42);
    const auto cycles = runProgram({0xB7, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LAX_Absolute) {
    // LAX $0300 ; Load A and X from absolute address
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xAF, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LAX_AbsoluteY) {
    // LAX $0300,Y ; Load A and X from absolute + Y
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0305, 0x42);
    const auto cycles = runProgram({0xBF, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LAX_IndirectX) {
    // LAX ($10,X) ; Load A and X from indirect X
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xA3, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

TEST_F(CPUTestBase, LAX_IndirectY) {
    // LAX ($10),Y ; Load A and X from indirect Y
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x42);
    const auto cycles = runProgram({0xB3, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
}

// ==================== LAS (Unofficial) ====================

TEST_F(CPUTestBase, LAS_AbsoluteY) {
    // LAS $0300,Y ; A = X = SP = SP AND memory
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0305, 0x0F);
    const auto cycles = runProgram({0xBB, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x0F);
    EXPECT_EQ(emulator.cpu().registers().X, 0x0F);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x0F);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LAS_AbsoluteY_Zero) {
    // LAS $0300,Y ; Result is zero
    emulator.cpu().registers().SP = 0xF0;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0305, 0x0F);
    const auto cycles = runProgram({0xBB, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_EQ(emulator.cpu().registers().X, 0x00);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, LAS_AbsoluteY_Negative) {
    // LAS $0300,Y ; Result is negative
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0305, 0x80);
    const auto cycles = runProgram({0xBB, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x80);
    EXPECT_EQ(emulator.cpu().registers().X, 0x80);
    EXPECT_EQ(emulator.cpu().registers().SP, 0x80);
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}
