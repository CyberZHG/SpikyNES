#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <bitset>

#include "core/emulator.h"
#include "../../ppu/test_ppu_utils.h"

using namespace nes_core;

namespace {

    struct CPUState {
        std::uint16_t PC = 0;
        std::uint8_t A = 0;
        std::uint8_t X = 0;
        std::uint8_t Y = 0;
        std::uint8_t P = 0;
        std::uint8_t SP = 0;
        int cycles = 0;
    };

    std::uint8_t parseHex8(const std::string& s) {
        return static_cast<std::uint8_t>(std::stoul(s, nullptr, 16));
    }

    std::uint16_t parseHex16(const std::string& s) {
        return static_cast<std::uint16_t>(std::stoul(s, nullptr, 16));
    }

    CPUState parseLogLine(const std::string& line) {
        CPUState state;
        state.PC = parseHex16(line.substr(0, 4));
        auto findVal = [&line](const std::string& prefix) -> std::uint8_t {
            if (const auto pos = line.find(prefix); pos != std::string::npos) {
                return parseHex8(line.substr(pos + prefix.length(), 2));
            }
            return 0;
        };
        state.A = findVal("A:");
        state.X = findVal("X:");
        state.Y = findVal("Y:");
        state.P = findVal("P:");
        state.SP = findVal("SP:");
        if (const auto pos = line.find("CYC:"); pos != std::string::npos) {
            state.cycles = std::stoi(line.substr(pos + 4));
        }
        return state;
    }

    std::vector<CPUState> loadNestestLog(const std::string& path) {
        std::vector<CPUState> states;
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                states.push_back(parseLogLine(line));
            }
        }
        return states;
    }

    std::string formatState(const CPUState& s) {
        std::ostringstream oss;
        oss << "PC:" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << s.PC
            << " A:" << std::setw(2) << static_cast<int>(s.A)
            << " X:" << std::setw(2) << static_cast<int>(s.X)
            << " Y:" << std::setw(2) << static_cast<int>(s.Y)
            << " P:" << std::setw(2) << static_cast<int>(s.P)
            << " SP:" << std::setw(2) << static_cast<int>(s.SP);
        return oss.str();
    }

}

class NestestTest : public testing::Test {
protected:
    Emulator emulator;

    CPUState getCurrentState() const {
        CPUState s;
        s.PC = emulator.cpu().registers().PC;
        s.A = emulator.cpu().registers().A;
        s.X = emulator.cpu().registers().X;
        s.Y = emulator.cpu().registers().Y;
        s.P = emulator.cpu().registers().P;
        s.SP = emulator.cpu().registers().SP;
        return s;
    }
};

TEST_F(NestestTest, RunNestest) {
    const std::string romPath = "../tests/roms/cpu/nestest.nes";
    const std::string logPath = "../tests/roms/cpu/nestest.log";

    ASSERT_TRUE(emulator.loadROM(romPath)) << "Failed to load nestest.nes";
    emulator.cpu().registers().PC = 0xC000;
    auto expectedStates = loadNestestLog(logPath);
    ASSERT_FALSE(expectedStates.empty()) << "Failed to load nestest.log";
    std::uint64_t totalCycles = 7;
    for (size_t i = 0; i < expectedStates.size(); ++i) {
        const auto& expected = expectedStates[i];
        const auto actual = getCurrentState();
        ASSERT_EQ(actual.PC, expected.PC)
            << "Mismatch at instruction " << i << "\n"
            << "Expected: " << formatState(expected) << "\n"
            << "  Actual: " << formatState(actual);
        ASSERT_EQ(actual.A, expected.A)
            << "A mismatch at instruction " << i << " (PC=" << std::hex << actual.PC << ")\n"
            << "Expected A: " << std::hex << static_cast<int>(expected.A) << "\n"
            << "  Actual A: " << std::hex << static_cast<int>(actual.A);
        ASSERT_EQ(actual.X, expected.X)
            << "X mismatch at instruction " << i << " (PC=" << std::hex << actual.PC << ")\n"
            << "Expected X: " << std::hex << static_cast<int>(expected.X) << "\n"
            << "  Actual X: " << std::hex << static_cast<int>(actual.X);
        ASSERT_EQ(actual.Y, expected.Y)
            << "Y mismatch at instruction " << i << " (PC=" << std::hex << actual.PC << ")\n"
            << "Expected Y: " << std::hex << static_cast<int>(expected.Y) << "\n"
            << "  Actual Y: " << std::hex << static_cast<int>(actual.Y);
        ASSERT_EQ(actual.P, expected.P)
            << "P mismatch at instruction " << i << " (PC=" << std::hex << actual.PC << ")\n"
            << "Expected P: " << std::bitset<8>(expected.P) << "\n"
            << "  Actual P: " << std::bitset<8>(actual.P);
        ASSERT_EQ(actual.SP, expected.SP)
            << "SP mismatch at instruction " << i << " (PC=" << std::hex << actual.PC << ")";
        ASSERT_EQ(totalCycles, expected.cycles)
            << "Cycles mismatch at instruction " << i << " (PC=" << std::hex << actual.PC << ")\n"
            << "Expected cycles: " << std::dec << expected.cycles << "\n"
            << "  Actual cycles: " << std::dec << totalCycles;
        totalCycles += emulator.execOp();
    }
    const auto mem02 = emulator.cpu().cpuRead(0x02);
    const auto mem03 = emulator.cpu().cpuRead(0x03);
    EXPECT_EQ(mem02, 0x00)
        << "Official opcode tests failed with code: " << std::hex << static_cast<int>(mem02);
    EXPECT_EQ(mem03, 0x00)
        << "Unofficial opcode tests failed with code: " << std::hex << static_cast<int>(mem03);
}

TEST_F(NestestTest, NestestAll) {
    const std::string romPath = "../tests/roms/cpu/nestest.nes";
    ASSERT_TRUE(emulator.loadROM(romPath));
    while (!emulator.cpu().halted() && emulator.ppu().frame() < 10) {
        emulator.step();
    }
    emulator.controller1().press(Controller::Button::Start);
    while (!emulator.cpu().halted() && emulator.ppu().frame() < 30) {
        emulator.step();
    }
    const auto filepath = saveFrameBuffer(emulator.ppu());
}