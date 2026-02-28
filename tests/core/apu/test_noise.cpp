#include <gtest/gtest.h>
#include "core/emulator.h"

using namespace nes_core;

class NoiseTest : public ::testing::Test {
protected:
    void SetUp() override {
        emulator.reset();
    }

    Emulator emulator;
};

TEST_F(NoiseTest, RegisterWrite_LengthCounterHalt) {
    emulator.apu().write(0x400C, 0b00100000);
    EXPECT_TRUE(emulator.apu().registers().noise.lengthCounterHalted);

    emulator.apu().write(0x400C, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().noise.lengthCounterHalted);
}

TEST_F(NoiseTest, RegisterWrite_ConstantVolume) {
    emulator.apu().write(0x400C, 0b00010000);
    EXPECT_TRUE(emulator.apu().registers().noise.constantVolume);

    emulator.apu().write(0x400C, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().noise.constantVolume);
}

TEST_F(NoiseTest, RegisterWrite_Volume) {
    emulator.apu().write(0x400C, 0b00001111);
    EXPECT_EQ(emulator.apu().registers().noise.volume, 15);

    emulator.apu().write(0x400C, 0b00000101);
    EXPECT_EQ(emulator.apu().registers().noise.volume, 5);
}

TEST_F(NoiseTest, RegisterWrite_Mode) {
    emulator.apu().write(0x400E, 0b10000000);
    EXPECT_TRUE(emulator.apu().registers().noise.mode);

    emulator.apu().write(0x400E, 0b00000000);
    EXPECT_FALSE(emulator.apu().registers().noise.mode);
}

TEST_F(NoiseTest, RegisterWrite_PeriodIndex) {
    emulator.apu().write(0x400E, 0b00001111);
    EXPECT_EQ(emulator.apu().registers().noise.periodIndex, 15);

    emulator.apu().write(0x400E, 0b00000101);
    EXPECT_EQ(emulator.apu().registers().noise.periodIndex, 5);
}

TEST_F(NoiseTest, RegisterWrite_LengthCounterLoad) {
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 10);

    emulator.apu().write(0x400F, 0b11111000);
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 30);
}

TEST_F(NoiseTest, RegisterWrite_EnvelopeRestart) {
    emulator.apu().registers().noise.envelopeStart = false;
    emulator.apu().write(0x400F, 0x00);
    EXPECT_TRUE(emulator.apu().registers().noise.envelopeStart);
}

TEST_F(NoiseTest, Envelope_ConstantVolume) {
    emulator.apu().write(0x400C, 0b00011010);
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);
    emulator.apu().registers().noise.shiftRegister = 0x4000;
    EXPECT_EQ(emulator.apu().registers().noise.output(), 10);
}

TEST_F(NoiseTest, Envelope_Decay) {
    emulator.apu().write(0x400C, 0b00001111);
    emulator.apu().write(0x400F, 0b00000000);
    emulator.apu().write(0x4015, 0b00001000);

    emulator.apu().registers().noise.envelopeStep();
    EXPECT_EQ(emulator.apu().registers().noise.envelopeDecayLevel, 15);

    for (int i = 0; i < 16; ++i) {
        emulator.apu().registers().noise.envelopeStep();
    }
    EXPECT_EQ(emulator.apu().registers().noise.envelopeDecayLevel, 14);
}

TEST_F(NoiseTest, Envelope_Loop) {
    emulator.apu().write(0x400C, 0b00101111);
    emulator.apu().write(0x400F, 0b00000000);
    emulator.apu().write(0x4015, 0b00001000);

    emulator.apu().registers().noise.envelopeStep();
    for (int i = 0; i < 16 * 16; ++i) {
        emulator.apu().registers().noise.envelopeStep();
    }
    EXPECT_EQ(emulator.apu().registers().noise.envelopeDecayLevel, 15);
}

TEST_F(NoiseTest, LengthCounter_Decrement) {
    emulator.apu().write(0x400C, 0b00000000);
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);

    emulator.apu().registers().noise.lengthCounterStep();
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 9);

    for (int i = 0; i < 9; ++i) {
        emulator.apu().registers().noise.lengthCounterStep();
    }
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 0);

    emulator.apu().registers().noise.lengthCounterStep();
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 0);
}

TEST_F(NoiseTest, LengthCounter_Halt) {
    emulator.apu().write(0x400C, 0b00100000);
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);

    for (int i = 0; i < 20; ++i) {
        emulator.apu().registers().noise.lengthCounterStep();
    }
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 10);
}

TEST_F(NoiseTest, LFSR_NormalMode_Step) {
    emulator.apu().registers().noise.shiftRegister = 1;
    emulator.apu().registers().noise.timer = 0;
    emulator.apu().registers().noise.timerStep();
    EXPECT_EQ(emulator.apu().registers().noise.shiftRegister, 0x4000);
}

TEST_F(NoiseTest, LFSR_NormalMode_FeedbackZero) {
    emulator.apu().registers().noise.shiftRegister = 3;
    emulator.apu().registers().noise.timer = 0;
    emulator.apu().registers().noise.timerStep();
    EXPECT_EQ(emulator.apu().registers().noise.shiftRegister, 1);
}

TEST_F(NoiseTest, LFSR_ShortMode_UsesBit6) {
    emulator.apu().write(0x400E, 0b10000000);
    emulator.apu().registers().noise.shiftRegister = 0b01000001;
    emulator.apu().registers().noise.timer = 0;
    emulator.apu().registers().noise.timerStep();
    EXPECT_EQ(emulator.apu().registers().noise.shiftRegister, 0b00100000);
}

TEST_F(NoiseTest, LFSR_ShortMode_VsNormalMode) {
    emulator.apu().write(0x400E, 0b00000000);
    emulator.apu().registers().noise.shiftRegister = 0b01000001;
    emulator.apu().registers().noise.timer = 0;
    emulator.apu().registers().noise.timerStep();
    EXPECT_EQ(emulator.apu().registers().noise.shiftRegister, 0x4020);
}

TEST_F(NoiseTest, LFSR_Timer_ReloadsPeriod) {
    emulator.apu().write(0x400E, 0b00000001);
    emulator.apu().registers().noise.timer = 0;
    emulator.apu().registers().noise.timerStep();
    EXPECT_EQ(emulator.apu().registers().noise.timer, 8);
}

TEST_F(NoiseTest, Output_SilencedWhenBit0Set) {
    emulator.apu().write(0x400C, 0b00011111);
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);

    emulator.apu().registers().noise.shiftRegister = 1;
    EXPECT_EQ(emulator.apu().registers().noise.output(), 0);

    emulator.apu().registers().noise.shiftRegister = 0x4000;
    EXPECT_GT(emulator.apu().registers().noise.output(), 0);
}

TEST_F(NoiseTest, Output_SilencedByLengthCounter) {
    emulator.apu().write(0x400C, 0b00011111);
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);
    emulator.apu().registers().noise.shiftRegister = 0x4000;

    EXPECT_GT(emulator.apu().registers().noise.output(), 0);

    emulator.apu().registers().noise.lengthCounter = 0;
    EXPECT_EQ(emulator.apu().registers().noise.output(), 0);
}

TEST_F(NoiseTest, Output_ConstantVolumeVsEnvelope) {
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);
    emulator.apu().registers().noise.shiftRegister = 0x4000;

    emulator.apu().write(0x400C, 0b00011010);
    EXPECT_EQ(emulator.apu().registers().noise.output(), 10);

    emulator.apu().write(0x400C, 0b00001010);
    emulator.apu().registers().noise.envelopeDecayLevel = 7;
    EXPECT_EQ(emulator.apu().registers().noise.output(), 7);
}

TEST_F(NoiseTest, Enable_ProducesOutput) {
    emulator.apu().write(0x400C, 0b00011111);
    emulator.apu().write(0x400E, 0b00000001);
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b00000000);
    emulator.apu().registers().noise.shiftRegister = 0x4000;
    emulator.apu().step();
    EXPECT_GT(emulator.apu().outputBuffer(), 0.0f);

    emulator.apu().write(0x4015, 0b00000000);
    emulator.apu().step();
    EXPECT_EQ(emulator.apu().outputBuffer(), 0.0f);
}

TEST_F(NoiseTest, Enable_ClearsLengthCounter) {
    emulator.apu().write(0x4015, 0b00001000);
    emulator.apu().write(0x400F, 0b11111000);
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 30);

    emulator.apu().write(0x4015, 0b00000000);
    EXPECT_EQ(emulator.apu().registers().noise.lengthCounter, 0);
}
