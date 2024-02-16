#include "test_cpu_utils.h"

// ==================== BRK ====================

TEST_F(CPUTestBase, BRK) {
    // BRK ; Push PC+2, push P with B=1, jump to IRQ vector
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00110100;
    emulator.bus().cpuWrite(0xFFFE, 0x00);
    emulator.bus().cpuWrite(0xFFFF, 0x04);
    const auto cycles = runProgram({0x00}, 1);
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0400);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFC);
    EXPECT_TRUE(emulator.cpu().registers().I());
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), (CPU_PROGRAM_BEGIN + 2) >> 8);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FE), (CPU_PROGRAM_BEGIN + 2) & 0xFF);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FD) & 0b00110000, 0b00110000);
}

// ==================== RTI ====================

TEST_F(CPUTestBase, RTI) {
    // RTI ; Pull P, pull PC
    emulator.cpu().registers().SP = 0xFC;
    emulator.cpu().registers().P = 0b00110100;
    emulator.cpu().cpuWrite(0x01FD, 0b11000011);
    emulator.cpu().cpuWrite(0x01FE, 0x00);
    emulator.cpu().cpuWrite(0x01FF, 0x03);
    const auto cycles = runProgram({0x40}, 1);
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0300);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
    EXPECT_TRUE(emulator.cpu().registers().N());
    EXPECT_TRUE(emulator.cpu().registers().V());
    EXPECT_TRUE(emulator.cpu().registers().C());
    EXPECT_TRUE(emulator.cpu().registers().Z());
}

TEST_F(CPUTestBase, BRK_RTI_RoundTrip) {
    // BRK then RTI should return to instruction after BRK padding byte
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00110100;
    emulator.bus().cpuWrite(0xFFFE, 0x10);
    emulator.bus().cpuWrite(0xFFFF, 0x02);
    emulator.cpu().cpuWrite(0x0210, 0x40);
    const auto cycles = runProgram({0x00, 0x00, 0xEA}, 3);
    // BRK(7) + RTI(6) + NOP(2) = 15 cycles
    EXPECT_EQ(cycles, 15);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 3);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
}

// ==================== JAM (Unofficial) ====================

TEST_F(CPUTestBase, JAM) {
    // JAM ; Halt the CPU
    EXPECT_FALSE(emulator.cpu().halted());
    const auto cycles = runProgram({0x02}, 1);
    EXPECT_EQ(cycles, 2);
    EXPECT_TRUE(emulator.cpu().halted());
}

// ==================== NMI ====================

TEST_F(CPUTestBase, NMI) {
    // NMI ; Push PC, push P with B=0, jump to NMI vector
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00100100;
    emulator.cpu().registers().PC = 0x0300;
    emulator.bus().cpuWrite(0xFFFA, 0x00);
    emulator.bus().cpuWrite(0xFFFB, 0x05);
    emulator.cpu().nmi();
    const auto cycles = emulator.execOp();
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0500);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFC);
    EXPECT_TRUE(emulator.cpu().registers().I());
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), 0x03);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FE), 0x00);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FD) & 0b00110000, 0b00100000);
}

TEST_F(CPUTestBase, NMI_IgnoresIFlag) {
    // NMI is non-maskable, fires even when I flag is set
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00100100;
    emulator.cpu().registers().P |= 0x04;
    emulator.cpu().registers().PC = 0x0300;
    emulator.bus().cpuWrite(0xFFFA, 0x00);
    emulator.bus().cpuWrite(0xFFFB, 0x05);
    emulator.cpu().nmi();
    const auto cycles = emulator.execOp();
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0500);
}

// ==================== IRQ ====================

TEST_F(CPUTestBase, IRQ) {
    // IRQ ; Push PC, push P with B=0, jump to IRQ vector
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00100000;
    emulator.cpu().registers().PC = 0x0300;
    emulator.bus().cpuWrite(0xFFFE, 0x00);
    emulator.bus().cpuWrite(0xFFFF, 0x04);
    emulator.cpu().irq();
    const auto cycles = emulator.execOp();
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0400);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFC);
    EXPECT_TRUE(emulator.cpu().registers().I());
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FF), 0x03);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FE), 0x00);
    EXPECT_EQ(emulator.cpu().cpuRead(0x01FD) & 0b00110000, 0b00100000);
}

TEST_F(CPUTestBase, IRQ_MaskedByIFlag) {
    // IRQ is ignored when I flag is set
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00100100;
    emulator.cpu().registers().PC = CPU_PROGRAM_BEGIN;
    emulator.cpu().cpuWrite(CPU_PROGRAM_BEGIN, 0xEA);
    emulator.bus().cpuWrite(0xFFFE, 0x00);
    emulator.bus().cpuWrite(0xFFFF, 0x04);
    emulator.cpu().irq();
    const auto cycles = emulator.execOp();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(emulator.cpu().registers().PC, CPU_PROGRAM_BEGIN + 1);
    EXPECT_EQ(emulator.cpu().registers().SP, 0xFF);
}

TEST_F(CPUTestBase, NMI_PriorityOverIRQ) {
    // NMI has priority over IRQ
    emulator.cpu().registers().SP = 0xFF;
    emulator.cpu().registers().P = 0b00100000;
    emulator.cpu().registers().PC = 0x0300;
    emulator.bus().cpuWrite(0xFFFA, 0x00);
    emulator.bus().cpuWrite(0xFFFB, 0x05);
    emulator.bus().cpuWrite(0xFFFE, 0x00);
    emulator.bus().cpuWrite(0xFFFF, 0x04);
    emulator.cpu().nmi();
    emulator.cpu().irq();
    const auto cycles = emulator.execOp();
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(emulator.cpu().registers().PC, 0x0500);
}
