#include "ui/sdl_emulator.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    nes_ui::SDLEmulator emulator;
    if (!emulator.create()) {
        return 1;
    }
    while (emulator.running()) {
        emulator.pollEvents();
        emulator.renderMenu();
        emulator.render();
        emulator.delayFrame();
    }
    return 0;
}