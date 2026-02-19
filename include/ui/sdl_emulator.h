#ifndef SPIKYNES_SDL_EMULATOR_H
#define SPIKYNES_SDL_EMULATOR_H

#include <SDL3/SDL.h>
#include <imgui.h>
#include "core/emulator.h"

namespace nes_ui {
    class SDLEmulator {
    public:
        SDLEmulator() = default;
        ~SDLEmulator();

        enum class EmulationState {
            Idle, Running, Paused,
        };

        [[nodiscard]] bool running() const { return running_; }

        bool create();
        void destroy();

        void pollEvents();
        void renderMenu();
        void render();
        void delayFrame();

    private:
        bool running_ = true;
        bool pendingFileDialog_ = false;
        EmulationState state_ = EmulationState::Idle;
        std::uint64_t perfCounter_ = 0;
        nes_core::Emulator emulator_{};

        SDL_Window* window_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
        SDL_Texture* texture_ = nullptr;
        bool imguiInitialized_ = false;

        static void openFileCallback(void *userdata, const char * const *files, int filter);
    };
}


#endif //SPIKYNES_SDL_EMULATOR_H