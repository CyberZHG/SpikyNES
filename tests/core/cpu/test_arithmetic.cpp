#include "test_cpu_utils.h"

// ==================== ADC ====================

TEST_F(CPUTestBase, ADC_Immediate) {
    // ADC #$10 ; A = A + M + C
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x69, 0x10}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
    EXPECT_FALSE(emulator.cpu().registers().V());
}

TEST_F(CPUTestBase, ADC_Immediate_WithCarryIn) {
    // ADC #$10 ; A = A + M + C, C=1
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0x69, 0x10}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x31);
    EXPECT_FALSE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, ADC_Immediate_CarryOut) {
    // ADC #$01 ; A = 0xFF + 0x01 = 0x100, C=1
    emulator.cpu().registers().A = 0xFF;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x69, 0x01}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, ADC_Immediate_Overflow) {
    // ADC #$50 ; 0x50 + 0x50 = 0xA0 (positive + positive = negative, overflow)
    emulator.cpu().registers().A = 0x50;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x69, 0x50}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0xA0);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().V());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ADC_Immediate_OverflowNegative) {
    // ADC #$D0 ; 0x90 + 0xD0 = 0x160 (negative + negative = positive, overflow)
    emulator.cpu().registers().A = 0x90;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x69, 0xD0}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x60);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().V());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, ADC_ZeroPage) {
    // ADC $10
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().write(0x0010, 0x10);
    const auto cycles = runProgram({0x65, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
}

TEST_F(CPUTestBase, ADC_ZeroPageX) {
    // ADC $10,X
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().write(0x0015, 0x10);
    const auto cycles = runProgram({0x75, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
}

TEST_F(CPUTestBase, ADC_Absolute) {
    // ADC $0300
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().write(0x0300, 0x10);
    const auto cycles = runProgram({0x6D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
}

TEST_F(CPUTestBase, ADC_AbsoluteX) {
    // ADC $0300,X
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().write(0x0310, 0x10);
    const auto cycles = runProgram({0x7D, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
}

TEST_F(CPUTestBase, ADC_AbsoluteY) {
    // ADC $0300,Y
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().write(0x0310, 0x10);
    const auto cycles = runProgram({0x79, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
}

TEST_F(CPUTestBase, ADC_IndirectX) {
    // ADC ($10,X)
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x10);
    const auto cycles = runProgram({0x61, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
}

TEST_F(CPUTestBase, ADC_IndirectY) {
    // ADC ($10),Y
    emulator.cpu().registers().A = 0x20;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x10);
    const auto cycles = runProgram({0x71, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(emulator.cpu().registers().A, 0x30);
}

// ==================== SBC ====================

TEST_F(CPUTestBase, SBC_Immediate) {
    // SBC #$10 ; A = A - M - (1 - C), C=1 means no borrow
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0xE9, 0x10}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x20);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
    EXPECT_FALSE(emulator.cpu().registers().V());
}

TEST_F(CPUTestBase, SBC_Immediate_WithBorrow) {
    // SBC #$10 ; A = A - M - 1, C=0 means borrow
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0xE9, 0x10}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x1F);
    EXPECT_TRUE(emulator.cpu().registers().C());
}

TEST_F(CPUTestBase, SBC_Immediate_BorrowOut) {
    // SBC #$01 ; A = 0x00 - 0x01 = 0xFF, borrow out
    emulator.cpu().registers().A = 0x00;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0xE9, 0x01}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0xFF);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, SBC_Immediate_Zero) {
    // SBC #$30 ; A = 0x30 - 0x30 = 0x00
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0xE9, 0x30}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x00);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, SBC_Immediate_Overflow) {
    // SBC #$B0 ; 0x50 - 0xB0 (positive - negative = negative, overflow)
    emulator.cpu().registers().A = 0x50;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0xE9, 0xB0}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0xA0);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().V());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, SBC_Immediate_OverflowNegative) {
    // SBC #$70 ; 0xD0 - 0x70 (negative - positive = positive, overflow)
    emulator.cpu().registers().A = 0xD0;
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0xE9, 0x70}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x60);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().V());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, SBC_ZeroPage) {
    // SBC $10
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().P |= 0x01;
    emulator.cpu().write(0x0010, 0x10);
    const auto cycles = runProgram({0xE5, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().A, 0x20);
}

TEST_F(CPUTestBase, SBC_Absolute) {
    // SBC $0300
    emulator.cpu().registers().A = 0x30;
    emulator.cpu().registers().P |= 0x01;
    emulator.cpu().write(0x0300, 0x10);
    const auto cycles = runProgram({0xED, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().A, 0x20);
}

// ==================== CMP ====================

TEST_F(CPUTestBase, CMP_Immediate_Equal) {
    // CMP #$42 ; A == M
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0xC9, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CMP_Immediate_Greater) {
    // CMP #$30 ; A > M
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0xC9, 0x30}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CMP_Immediate_Less) {
    // CMP #$50 ; A < M
    emulator.cpu().registers().A = 0x42;
    const auto cycles = runProgram({0xC9, 0x50}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().A, 0x42);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CMP_ZeroPage) {
    // CMP $10
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xC5, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CMP_ZeroPageX) {
    // CMP $10,X
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x42);
    const auto cycles = runProgram({0xD5, 0x10}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CMP_Absolute) {
    // CMP $0300
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xCD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CMP_AbsoluteX) {
    // CMP $0300,X
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x10;
    emulator.cpu().write(0x0310, 0x42);
    const auto cycles = runProgram({0xDD, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CMP_AbsoluteY) {
    // CMP $0300,Y
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().Y = 0x10;
    emulator.cpu().write(0x0310, 0x42);
    const auto cycles = runProgram({0xD9, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CMP_IndirectX) {
    // CMP ($10,X)
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().X = 0x05;
    emulator.cpu().write(0x0015, 0x00);
    emulator.cpu().write(0x0016, 0x03);
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xC1, 0x10}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CMP_IndirectY) {
    // CMP ($10),Y
    emulator.cpu().registers().A = 0x42;
    emulator.cpu().registers().Y = 0x05;
    emulator.cpu().write(0x0010, 0x00);
    emulator.cpu().write(0x0011, 0x03);
    emulator.cpu().write(0x0305, 0x42);
    const auto cycles = runProgram({0xD1, 0x10}, 1);
    EXPECT_EQ(cycles, 5);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

// ==================== CPX ====================

TEST_F(CPUTestBase, CPX_Immediate_Equal) {
    // CPX #$42 ; X == M
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0xE0, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().X, 0x42);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CPX_Immediate_Greater) {
    // CPX #$30 ; X > M
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0xE0, 0x30}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CPX_Immediate_Less) {
    // CPX #$50 ; X < M
    emulator.cpu().registers().X = 0x42;
    const auto cycles = runProgram({0xE0, 0x50}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CPX_ZeroPage) {
    // CPX $10
    emulator.cpu().registers().X = 0x42;
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xE4, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CPX_Absolute) {
    // CPX $0300
    emulator.cpu().registers().X = 0x42;
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xEC, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

// ==================== CPY ====================

TEST_F(CPUTestBase, CPY_Immediate_Equal) {
    // CPY #$42 ; Y == M
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0xC0, 0x42}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
    EXPECT_EQ(emulator.cpu().registers().Y, 0x42);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CPY_Immediate_Greater) {
    // CPY #$30 ; Y > M
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0xC0, 0x30}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_FALSE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CPY_Immediate_Less) {
    // CPY #$50 ; Y < M
    emulator.cpu().registers().Y = 0x42;
    const auto cycles = runProgram({0xC0, 0x50}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_FALSE(emulator.cpu().registers().C());
    EXPECT_FALSE(emulator.cpu().registers().Z());
    EXPECT_TRUE(emulator.cpu().registers().N());
}

TEST_F(CPUTestBase, CPY_ZeroPage) {
    // CPY $10
    emulator.cpu().registers().Y = 0x42;
    emulator.cpu().write(0x0010, 0x42);
    const auto cycles = runProgram({0xC4, 0x10}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, CPY_Absolute) {
    // CPY $0300
    emulator.cpu().registers().Y = 0x42;
    emulator.cpu().write(0x0300, 0x42);
    const auto cycles = runProgram({0xCC, 0x00, 0x03}, 1);
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(emulator.cpu().registers().Z());
}
