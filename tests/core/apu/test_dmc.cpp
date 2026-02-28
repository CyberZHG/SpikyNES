#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class DMCTest : public ::testing::Test {
protected:
    void SetUp() override {
        emulator.reset();
    }

    Emulator emulator;
};

TEST_F(DMCTest, RegisterWrite_IRQEnabled) {
    emulator.apu().write(0x4010, 0b10000000);
    EXPECT_TRUE(emulator.apu().registers().dmc.irqEnabled);

    emulator.apu().write(0x4010, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().dmc.irqEnabled);
}

TEST_F(DMCTest, RegisterWrite_LoopEnabled) {
    emulator.apu().write(0x4010, 0b01000000);
    EXPECT_TRUE(emulator.apu().registers().dmc.loopEnabled);

    emulator.apu().write(0x4010, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().dmc.loopEnabled);
}

TEST_F(DMCTest, RegisterWrite_RateIndex) {
    emulator.apu().write(0x4010, 0b00001111);
    EXPECT_EQ(emulator.apu().registers().dmc.rateIndex, 15);

    emulator.apu().write(0x4010, 0b00000101);
    EXPECT_EQ(emulator.apu().registers().dmc.rateIndex, 5);
}

TEST_F(DMCTest, RegisterWrite_IRQClearedWhenDisabled) {
    emulator.apu().registers().dmc.irqFlag = true;
    emulator.apu().write(0x4010, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().dmc.irqFlag);
}

TEST_F(DMCTest, RegisterWrite_OutputLevel) {
    emulator.apu().write(0x4011, 0b01111111);
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 127);

    emulator.apu().write(0x4011, 0b01000000);
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 64);

    emulator.apu().write(0x4011, 0b11111111);
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 127);
}

TEST_F(DMCTest, RegisterWrite_SampleAddress) {
    emulator.apu().write(0x4012, 0xAB);
    EXPECT_EQ(emulator.apu().registers().dmc.sampleAddress, 0xAB);
}

TEST_F(DMCTest, RegisterWrite_SampleLength) {
    emulator.apu().write(0x4013, 0x5F);
    EXPECT_EQ(emulator.apu().registers().dmc.sampleLength, 0x5F);
}

TEST_F(DMCTest, Restart_CurrentAddress) {
    emulator.apu().write(0x4012, 0x10);
    emulator.apu().registers().dmc.restart();
    EXPECT_EQ(emulator.apu().registers().dmc.currentAddress, 0xC000 + (0x10 << 6));
}

TEST_F(DMCTest, Restart_CurrentAddressMax) {
    emulator.apu().write(0x4012, 0xFF);
    emulator.apu().registers().dmc.restart();
    EXPECT_EQ(emulator.apu().registers().dmc.currentAddress, 0xC000 + (0xFF << 6));
}

TEST_F(DMCTest, Restart_BytesRemaining) {
    emulator.apu().write(0x4013, 0x05);
    emulator.apu().registers().dmc.restart();
    EXPECT_EQ(emulator.apu().registers().dmc.bytesRemaining, (0x05 << 4) + 1);
}

TEST_F(DMCTest, Restart_BytesRemainingMin) {
    emulator.apu().write(0x4013, 0x00);
    emulator.apu().registers().dmc.restart();
    EXPECT_EQ(emulator.apu().registers().dmc.bytesRemaining, 1);
}

TEST_F(DMCTest, OutputUnit_TimerReload) {
    emulator.apu().write(0x4010, 0b00000001);
    emulator.apu().registers().dmc.timer = 0;
    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.timer, 379);
}

TEST_F(DMCTest, OutputUnit_IncrementByTwo) {
    emulator.apu().registers().dmc.silence = false;
    emulator.apu().registers().dmc.shiftRegister = 0xFF;
    emulator.apu().registers().dmc.outputLevel = 60;
    emulator.apu().registers().dmc.bitsRemaining = 2;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 62);
}

TEST_F(DMCTest, OutputUnit_DecrementByTwo) {
    emulator.apu().registers().dmc.silence = false;
    emulator.apu().registers().dmc.shiftRegister = 0xFE;
    emulator.apu().registers().dmc.outputLevel = 60;
    emulator.apu().registers().dmc.bitsRemaining = 2;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 58);
}

TEST_F(DMCTest, OutputUnit_ClampMax) {
    emulator.apu().registers().dmc.silence = false;
    emulator.apu().registers().dmc.shiftRegister = 0xFF;
    emulator.apu().registers().dmc.outputLevel = 126;
    emulator.apu().registers().dmc.bitsRemaining = 2;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 126);

    emulator.apu().registers().dmc.outputLevel = 125;
    emulator.apu().registers().dmc.timer = 0;
    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 127);
}

TEST_F(DMCTest, OutputUnit_ClampMin) {
    emulator.apu().registers().dmc.silence = false;
    emulator.apu().registers().dmc.shiftRegister = 0xFE;
    emulator.apu().registers().dmc.outputLevel = 1;
    emulator.apu().registers().dmc.bitsRemaining = 2;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 1);

    emulator.apu().registers().dmc.shiftRegister = 0xFE;
    emulator.apu().registers().dmc.outputLevel = 2;
    emulator.apu().registers().dmc.timer = 0;
    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 0);
}

TEST_F(DMCTest, OutputUnit_SilenceSkipsOutput) {
    emulator.apu().registers().dmc.silence = true;
    emulator.apu().registers().dmc.shiftRegister = 0xFF;
    emulator.apu().registers().dmc.outputLevel = 60;
    emulator.apu().registers().dmc.bitsRemaining = 2;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 60);
}

TEST_F(DMCTest, OutputUnit_ShiftRight) {
    emulator.apu().registers().dmc.silence = false;
    emulator.apu().registers().dmc.shiftRegister = 0b10110110;
    emulator.apu().registers().dmc.outputLevel = 60;
    emulator.apu().registers().dmc.bitsRemaining = 2;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.shiftRegister, 0b01011011);
}

TEST_F(DMCTest, OutputUnit_BitsRemainingDecrement) {
    emulator.apu().registers().dmc.silence = true;
    emulator.apu().registers().dmc.bitsRemaining = 5;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_EQ(emulator.apu().registers().dmc.bitsRemaining, 4);
}

TEST_F(DMCTest, OutputUnit_LoadSampleBuffer) {
    emulator.apu().registers().dmc.silence = false;
    emulator.apu().registers().dmc.shiftRegister = 0x01;
    emulator.apu().registers().dmc.outputLevel = 60;
    emulator.apu().registers().dmc.bitsRemaining = 1;
    emulator.apu().registers().dmc.sampleBufferEmpty = false;
    emulator.apu().registers().dmc.sampleBuffer = 0b10101010;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();

    EXPECT_EQ(emulator.apu().registers().dmc.outputLevel, 62);
    EXPECT_EQ(emulator.apu().registers().dmc.shiftRegister, 0b10101010);
    EXPECT_EQ(emulator.apu().registers().dmc.bitsRemaining, 8);
    EXPECT_FALSE(emulator.apu().registers().dmc.silence);
    EXPECT_TRUE(emulator.apu().registers().dmc.sampleBufferEmpty);
}

TEST_F(DMCTest, OutputUnit_SilenceWhenBufferEmpty) {
    emulator.apu().registers().dmc.silence = false;
    emulator.apu().registers().dmc.bitsRemaining = 1;
    emulator.apu().registers().dmc.sampleBufferEmpty = true;
    emulator.apu().registers().dmc.timer = 0;

    emulator.apu().registers().dmc.timerStep();
    EXPECT_TRUE(emulator.apu().registers().dmc.silence);
    EXPECT_EQ(emulator.apu().registers().dmc.bitsRemaining, 8);
}

TEST_F(DMCTest, Output_ReturnsOutputLevel) {
    emulator.apu().registers().dmc.outputLevel = 0;
    EXPECT_EQ(emulator.apu().registers().dmc.output(), 0);

    emulator.apu().registers().dmc.outputLevel = 64;
    EXPECT_EQ(emulator.apu().registers().dmc.output(), 64);

    emulator.apu().registers().dmc.outputLevel = 127;
    EXPECT_EQ(emulator.apu().registers().dmc.output(), 127);
}

TEST_F(DMCTest, Output_DirectLoad) {
    emulator.apu().write(0x4011, 42);
    EXPECT_EQ(emulator.apu().registers().dmc.output(), 42);
}

TEST_F(DMCTest, Status_EnableRestartsWhenEmpty) {
    emulator.apu().write(0x4012, 0x10);
    emulator.apu().write(0x4013, 0x05);
    emulator.apu().registers().dmc.bytesRemaining = 0;

    emulator.apu().write(0x4015, 0b00010000);
    EXPECT_EQ(emulator.apu().registers().dmc.currentAddress, 0xC000 + (0x10 << 6));
    EXPECT_EQ(emulator.apu().registers().dmc.bytesRemaining, (0x05 << 4) + 1);
}

TEST_F(DMCTest, Status_DisableClearsBytesRemaining) {
    emulator.apu().registers().dmc.bytesRemaining = 50;
    emulator.apu().write(0x4015, 0b00000000);
    EXPECT_EQ(emulator.apu().registers().dmc.bytesRemaining, 0);
}

TEST_F(DMCTest, Status_EnableDoesNotRestartWhenBytesRemain) {
    emulator.apu().write(0x4012, 0x10);
    emulator.apu().write(0x4013, 0x05);
    emulator.apu().registers().dmc.bytesRemaining = 10;
    emulator.apu().registers().dmc.currentAddress = 0xDEAD;

    emulator.apu().write(0x4015, 0b00010000);
    EXPECT_EQ(emulator.apu().registers().dmc.currentAddress, 0xDEAD);
    EXPECT_EQ(emulator.apu().registers().dmc.bytesRemaining, 10);
}

TEST_F(DMCTest, Status_IRQFlagClearedOnWrite) {
    emulator.apu().registers().dmc.irqFlag = true;
    emulator.apu().write(0x4015, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().dmc.irqFlag);
}
