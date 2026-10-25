#include <vector>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "core/emulator.h"
using namespace emscripten;
using namespace nes_core;

namespace {

    constexpr double PPU_FREQUENCY = CPU_FREQUENCY_NTSC * 3.0;

    Controller& controllerOf(Emulator& emulator, const int player) {
        return player == 2 ? emulator.controller2() : emulator.controller1();
    }

    bool loadROM(Emulator& emulator, const val& data) {
        return emulator.loadROMFromMemory(convertJSArrayToNumberVector<std::uint8_t>(data));
    }

    val runFrame(Emulator& emulator, const double sampleRate) {
        const double cyclesPerSample = PPU_FREQUENCY / sampleRate;
        std::vector<float> samples;
        const auto startFrame = emulator.ppu().frame();
        auto sampleIndex = static_cast<std::int64_t>(static_cast<double>(emulator.ppu().ppuCycles()) / cyclesPerSample);
        while (emulator.ppu().frame() == startFrame && !emulator.cpu().halted()) {
            emulator.step();
            const auto index = static_cast<std::int64_t>(static_cast<double>(emulator.ppu().ppuCycles()) / cyclesPerSample);
            if (index != sampleIndex) {
                sampleIndex = index;
                samples.push_back(emulator.apu().outputBuffer());
            }
        }
        return val::global("Float32Array").new_(typed_memory_view(samples.size(), samples.data()));
    }

    val frameBufferRGBA(const Emulator& emulator) {
        const auto& frameBuffer = emulator.ppu().frameBuffer();
        std::vector<std::uint8_t> rgba(frameBuffer.size() * 4);
        for (std::size_t i = 0; i < frameBuffer.size(); ++i) {
            const auto color = frameBuffer[i];
            rgba[i * 4 + 0] = (color >> 16) & 0xFF;
            rgba[i * 4 + 1] = (color >> 8) & 0xFF;
            rgba[i * 4 + 2] = color & 0xFF;
            rgba[i * 4 + 3] = (color >> 24) & 0xFF;
        }
        return val::global("Uint8ClampedArray").new_(typed_memory_view(rgba.size(), rgba.data()));
    }

}

EMSCRIPTEN_BINDINGS(SpikyNesWASM) {
    constant("SCREEN_WIDTH", 256);
    constant("SCREEN_HEIGHT", 240);

    enum_<Controller::Button>("Button")
        .value("A", Controller::Button::A)
        .value("B", Controller::Button::B)
        .value("SELECT", Controller::Button::Select)
        .value("START", Controller::Button::Start)
        .value("UP", Controller::Button::Up)
        .value("DOWN", Controller::Button::Down)
        .value("LEFT", Controller::Button::Left)
        .value("RIGHT", Controller::Button::Right)
    ;
    class_<CPURegisters>("CPURegisters")
        .property("A", &CPURegisters::A)
        .property("X", &CPURegisters::X)
        .property("Y", &CPURegisters::Y)
        .property("P", &CPURegisters::P)
        .property("SP", &CPURegisters::SP)
        .property("PC", &CPURegisters::PC)
    ;
    class_<Emulator>("Emulator")
        .constructor<>()
        .function("loadROM", &loadROM)
        .function("reset", &Emulator::reset)
        .function("step", &Emulator::step)
        .function("execOp", &Emulator::execOp)
        .function("run", &Emulator::run)
        .function("_runFrame", &runFrame)
        .function("cpuRegisters", optional_override([](Emulator& self) -> CPURegisters& {
            return self.cpu().registers();
        }), return_value_policy::reference())
        .function("cpuRead", optional_override([](Emulator& self, const std::uint16_t addr) {
            return self.bus().cpuRead(addr);
        }))
        .function("cpuWrite", optional_override([](Emulator& self, const std::uint16_t addr, const std::uint8_t value) {
            self.bus().cpuWrite(addr, value);
        }))
        .function("cpuCycles", optional_override([](const Emulator& self) {
            return static_cast<double>(self.cpu().cycles());
        }))
        .function("frame", optional_override([](const Emulator& self) {
            return static_cast<double>(self.ppu().frame());
        }))
        .function("halted", optional_override([](const Emulator& self) { return self.cpu().halted(); }))
        .function("mapper", optional_override([](const Emulator& self) { return self.cartridge().header().mapper; }))
        .function("_press", optional_override([](Emulator& self, const Controller::Button button, const int player) {
            controllerOf(self, player).press(button);
        }))
        .function("_release", optional_override([](Emulator& self, const Controller::Button button, const int player) {
            controllerOf(self, player).release(button);
        }))
        .function("frameBuffer", &frameBufferRGBA)
        .function("audioSample", optional_override([](const Emulator& self) { return self.apu().outputBuffer(); }))
    ;
}
