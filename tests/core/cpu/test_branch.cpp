#include "test_cpu_utils.h"

// ==================== BCC ====================

TEST_F(CPUTestBase, BCC_NotTaken) {
    // BCC +2 ; Branch if Carry Clear, C=1 so not taken
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0x90, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BCC_Taken) {
    // BCC +2 ; Branch if Carry Clear, C=0 so taken
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x90, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}

TEST_F(CPUTestBase, BCC_Taken_PageCross) {
    // BCC crossing page boundary, taken adds +2 (branch + page cross)
    emulator.cpu().registers().P &= ~0x01;
    emulator.cpu().cpuWrite(0x02F0, 0x90);
    emulator.cpu().cpuWrite(0x02F1, 0x20);
    emulator.cpu().registers().PC = 0x02F0;
    int cycles = 0;
    const auto stepsStart = emulator.cpu().steps();
    while (emulator.cpu().steps() == stepsStart) {
        cycles += emulator.cpu().step();
    }
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0312);
}

TEST_F(CPUTestBase, BCC_NotTaken_PageCross_NoPenalty) {
    // BCC not taken should NOT add page cross penalty
    emulator.cpu().registers().P |= 0x01;
    emulator.cpu().cpuWrite(0x02F0, 0x90);
    emulator.cpu().cpuWrite(0x02F1, 0x20);
    emulator.cpu().registers().PC = 0x02F0;
    int cycles = 0;
    const auto stepsStart = emulator.cpu().steps();
    while (emulator.cpu().steps() == stepsStart) {
        cycles += emulator.cpu().step();
    }
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x02F2);
}

TEST_F(CPUTestBase, BCC_Backward) {
    // BCC -2 ; Backward branch
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0x90, 0xFE}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN);
}

// ==================== BCS ====================

TEST_F(CPUTestBase, BCS_NotTaken) {
    // BCS +2 ; Branch if Carry Set, C=0 so not taken
    emulator.cpu().registers().P &= ~0x01;
    const auto cycles = runProgram({0xB0, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BCS_Taken) {
    // BCS +2 ; Branch if Carry Set, C=1 so taken
    emulator.cpu().registers().P |= 0x01;
    const auto cycles = runProgram({0xB0, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}

// ==================== BEQ ====================

TEST_F(CPUTestBase, BEQ_NotTaken) {
    // BEQ +2 ; Branch if Equal (Z=1), Z=0 so not taken
    emulator.cpu().registers().P &= ~0x02;
    const auto cycles = runProgram({0xF0, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BEQ_Taken) {
    // BEQ +2 ; Branch if Equal (Z=1), Z=1 so taken
    emulator.cpu().registers().P |= 0x02;
    const auto cycles = runProgram({0xF0, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}

// ==================== BMI ====================

TEST_F(CPUTestBase, BMI_NotTaken) {
    // BMI +2 ; Branch if Minus (N=1), N=0 so not taken
    emulator.cpu().registers().P &= ~0x80;
    const auto cycles = runProgram({0x30, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BMI_Taken) {
    // BMI +2 ; Branch if Minus (N=1), N=1 so taken
    emulator.cpu().registers().P |= 0x80;
    const auto cycles = runProgram({0x30, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}

// ==================== BNE ====================

TEST_F(CPUTestBase, BNE_NotTaken) {
    // BNE +2 ; Branch if Not Equal (Z=0), Z=1 so not taken
    emulator.cpu().registers().P |= 0x02;
    const auto cycles = runProgram({0xD0, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BNE_Taken) {
    // BNE +2 ; Branch if Not Equal (Z=0), Z=0 so taken
    emulator.cpu().registers().P &= ~0x02;
    const auto cycles = runProgram({0xD0, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}

// ==================== BPL ====================

TEST_F(CPUTestBase, BPL_NotTaken) {
    // BPL +2 ; Branch if Plus (N=0), N=1 so not taken
    emulator.cpu().registers().P |= 0x80;
    const auto cycles = runProgram({0x10, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BPL_Taken) {
    // BPL +2 ; Branch if Plus (N=0), N=0 so taken
    emulator.cpu().registers().P &= ~0x80;
    const auto cycles = runProgram({0x10, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}

// ==================== BVC ====================

TEST_F(CPUTestBase, BVC_NotTaken) {
    // BVC +2 ; Branch if Overflow Clear, V=1 so not taken
    emulator.cpu().registers().P |= 0x40;
    const auto cycles = runProgram({0x50, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BVC_Taken) {
    // BVC +2 ; Branch if Overflow Clear, V=0 so taken
    emulator.cpu().registers().P &= ~0x40;
    const auto cycles = runProgram({0x50, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}

// ==================== BVS ====================

TEST_F(CPUTestBase, BVS_NotTaken) {
    // BVS +2 ; Branch if Overflow Set, V=0 so not taken
    emulator.cpu().registers().P &= ~0x40;
    const auto cycles = runProgram({0x70, 0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 2);
}

TEST_F(CPUTestBase, BVS_Taken) {
    // BVS +2 ; Branch if Overflow Set, V=1 so taken
    emulator.cpu().registers().P |= 0x40;
    const auto cycles = runProgram({0x70, 0x02}, 1);
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 4);
}
