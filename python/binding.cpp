#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include "core/emulator.h"
using namespace nes_core;

namespace py = pybind11;

namespace {

    constexpr double PPU_FREQUENCY = CPU_FREQUENCY_NTSC * 3.0;

    Controller& controllerOf(Emulator& emulator, const int player) {
        return player == 2 ? emulator.controller2() : emulator.controller1();
    }

    std::vector<float> runFrame(Emulator& emulator, const double sampleRate) {
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
        return samples;
    }

    py::bytes frameBufferRGBA(const Emulator& emulator) {
        const auto& frameBuffer = emulator.ppu().frameBuffer();
        std::string rgba(frameBuffer.size() * 4, '\0');
        for (std::size_t i = 0; i < frameBuffer.size(); ++i) {
            const auto color = frameBuffer[i];
            rgba[i * 4 + 0] = static_cast<char>((color >> 16) & 0xFF);
            rgba[i * 4 + 1] = static_cast<char>((color >> 8) & 0xFF);
            rgba[i * 4 + 2] = static_cast<char>(color & 0xFF);
            rgba[i * 4 + 3] = static_cast<char>((color >> 24) & 0xFF);
        }
        return py::bytes(rgba);
    }

}

PYBIND11_MODULE(_core, m, py::mod_gil_not_used()) {
    m.attr("SCREEN_WIDTH") = 256;
    m.attr("SCREEN_HEIGHT") = 240;

    py::enum_<Controller::Button>(m, "Button")
        .value("A", Controller::Button::A)
        .value("B", Controller::Button::B)
        .value("SELECT", Controller::Button::Select)
        .value("START", Controller::Button::Start)
        .value("UP", Controller::Button::Up)
        .value("DOWN", Controller::Button::Down)
        .value("LEFT", Controller::Button::Left)
        .value("RIGHT", Controller::Button::Right)
    ;
    py::class_<CPURegisters>(m, "CPURegisters")
        .def_readwrite("A", &CPURegisters::A)
        .def_readwrite("X", &CPURegisters::X)
        .def_readwrite("Y", &CPURegisters::Y)
        .def_readwrite("P", &CPURegisters::P)
        .def_readwrite("SP", &CPURegisters::SP)
        .def_readwrite("PC", &CPURegisters::PC)
    ;
    py::class_<Emulator>(m, "Emulator")
        .def(py::init<>())
        .def("load_rom", &Emulator::loadROM, py::arg("path"))
        .def("load_rom_from_bytes", [](Emulator& self, const py::bytes& data) {
            const auto view = static_cast<std::string_view>(data);
            return self.loadROMFromMemory(std::vector<std::uint8_t>(view.begin(), view.end()));
        }, py::arg("data"))
        .def("reset", &Emulator::reset)
        .def("step", &Emulator::step)
        .def("exec_op", &Emulator::execOp)
        .def("run", &Emulator::run, py::arg("milliseconds"))
        .def("run_frame", &runFrame, py::arg("sample_rate") = 44100.0)
        .def_property_readonly("cpu_registers", [](Emulator& self) -> CPURegisters& {
            return self.cpu().registers();
        }, py::return_value_policy::reference_internal)
        .def("cpu_read", [](Emulator& self, const std::uint16_t addr) { return self.bus().cpuRead(addr); }, py::arg("addr"))
        .def("cpu_write", [](Emulator& self, const std::uint16_t addr, const std::uint8_t value) {
            self.bus().cpuWrite(addr, value);
        }, py::arg("addr"), py::arg("value"))
        .def("cpu_cycles", [](const Emulator& self) { return self.cpu().cycles(); })
        .def("frame", [](const Emulator& self) { return self.ppu().frame(); })
        .def("halted", [](const Emulator& self) { return self.cpu().halted(); })
        .def("mapper", [](const Emulator& self) { return self.cartridge().header().mapper; })
        .def("press", [](Emulator& self, const Controller::Button button, const int player) {
            controllerOf(self, player).press(button);
        }, py::arg("button"), py::arg("player") = 1)
        .def("release", [](Emulator& self, const Controller::Button button, const int player) {
            controllerOf(self, player).release(button);
        }, py::arg("button"), py::arg("player") = 1)
        .def("frame_buffer", &frameBufferRGBA)
        .def("audio_sample", [](const Emulator& self) { return self.apu().outputBuffer(); })
    ;
}
