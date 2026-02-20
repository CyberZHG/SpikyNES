#include "ui/sdl_emulator.h"
#include "core/controller.h"

#include <iostream>
#include <cstdio>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

namespace nes_ui {
    SDLEmulator::~SDLEmulator() {
        destroy();
    }

    bool SDLEmulator::create() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        window_ = SDL_CreateWindow("SpikyNes", 256 * 3, 240 * 3, 0);
        if (!window_) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (!renderer_) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window_);
            SDL_Quit();
            return false;
        }

        texture_ = SDL_CreateTexture(
            renderer_,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            256,
            240
        );
        if (!texture_) {
            std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(renderer_);
            SDL_DestroyWindow(window_);
            SDL_Quit();
            return false;
        }

        SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_PIXELART);

        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_);
        ImGui_ImplSDLRenderer3_Init(renderer_);
        imguiInitialized_ = true;

        SDL_AudioSpec spec;
        spec.freq = 44100.0;
        spec.format = SDL_AUDIO_F32;
        spec.channels = 1;

        audioStream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
        if (!audioStream_) {
            std::cerr << "SDL_OpenAudioDeviceStream failed: " << SDL_GetError() << std::endl;
            return false;
        }

        audioDevice_ = SDL_GetAudioStreamDevice(audioStream_);
        SDL_ResumeAudioDevice(audioDevice_);

        perfCounter_ = SDL_GetPerformanceCounter();
        audioBuffer_.reserve(1024);
        return true;
    }

    void SDLEmulator::destroy() {
        if (imguiInitialized_) {
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            imguiInitialized_ = false;
        }

        if (audioStream_) {
            SDL_DestroyAudioStream(audioStream_);
            audioStream_ = nullptr;
        }
        if (texture_) {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        if (renderer_) {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
        if (window_) {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
        SDL_Quit();
    }

    void SDLEmulator::pollEvents() {
        if (pendingFileDialog_) {
            pendingFileDialog_ = false;
#ifdef __APPLE__
            FILE* fp = popen(
                "osascript"
                " -e 'try'"
                " -e 'POSIX path of (choose file of type {\"nes\"} with prompt \"Open NES ROM\")'"
                " -e 'end try'",
                "r"
            );
            if (fp) {
                char buf[4096] = {};
                if (fgets(buf, sizeof(buf), fp)) {
                    std::string path(buf);
                    if (!path.empty() && path.back() == '\n') path.pop_back();
                    if (!path.empty()) {
                        emulator_.loadROM(path);
                        state_ = EmulationState::Running;
                        perfCounter_ = SDL_GetPerformanceCounter();
                    }
                }
                pclose(fp);
            }
#else
            SDL_DialogFileFilter filters[] = {
                { "iNES Files", "*.nes" },
                { "All Files", "*" },
            };
            SDL_ShowOpenFileDialog(
                &SDLEmulator::openFileCallback,
                this,
                window_,
                filters,
                2,
                nullptr,
                false
            );
#endif
        }
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            switch (event.type) {
            case SDL_EVENT_KEY_DOWN: {
                const auto& key = event.key;
                switch (event.key.key) {
                    case SDLK_1:
                        emulator_.controller1().press(nes_core::Controller::Button::Select);
                        break;
                    case SDLK_2:
                        emulator_.controller1().press(nes_core::Controller::Button::Start);
                        break;
                    case SDLK_W:
                        emulator_.controller1().press(nes_core::Controller::Button::Up);
                        break;
                    case SDLK_S:
                        emulator_.controller1().press(nes_core::Controller::Button::Down);
                        break;
                    case SDLK_A:
                        emulator_.controller1().press(nes_core::Controller::Button::Left);
                        break;
                    case SDLK_D:
                        emulator_.controller1().press(nes_core::Controller::Button::Right);
                        break;
                    case SDLK_J:
                        emulator_.controller1().press(nes_core::Controller::Button::A);
                        break;
                    case SDLK_K:
                        emulator_.controller1().press(nes_core::Controller::Button::B);
                        break;
                    default:
                        break;
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                const auto& key = event.key;
                switch (event.key.key) {
                    case SDLK_1:
                        emulator_.controller1().release(nes_core::Controller::Button::Select);
                        break;
                    case SDLK_2:
                        emulator_.controller1().release(nes_core::Controller::Button::Start);
                        break;
                    case SDLK_W:
                        emulator_.controller1().release(nes_core::Controller::Button::Up);
                        break;
                    case SDLK_S:
                        emulator_.controller1().release(nes_core::Controller::Button::Down);
                        break;
                    case SDLK_A:
                        emulator_.controller1().release(nes_core::Controller::Button::Left);
                        break;
                    case SDLK_D:
                        emulator_.controller1().release(nes_core::Controller::Button::Right);
                        break;
                    case SDLK_J:
                        emulator_.controller1().release(nes_core::Controller::Button::A);
                        break;
                    case SDLK_K:
                        emulator_.controller1().release(nes_core::Controller::Button::B);
                        break;
                    default:
                        break;
                }
                break;
                }
            case SDL_EVENT_QUIT:
                running_ = false;
                break;
            default:
                break;
            }
        }
    }

    void SDLEmulator::renderMenu() {
        if (state_ == EmulationState::Idle) {
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Open")) {
                        pendingFileDialog_ = true;
                    }
                    if (ImGui::MenuItem("Exit")) {
                        running_ = false;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            ImGui::Render();
        }
    }

    void SDLEmulator::render() {
        constexpr double audioCycles = 1789773.0 / 44100.0;
        if (state_ == EmulationState::Running) {
            const auto current_time = SDL_GetPerformanceCounter();
            const auto frequency = static_cast<double>(SDL_GetPerformanceFrequency());
            const double elapsed = static_cast<double>(current_time - perfCounter_) * 1000.0 / frequency;
            perfCounter_ = SDL_GetPerformanceCounter();
            remainingLag_ += elapsed;
            while (remainingLag_ > 1000.0 / 60.0988) {
                remainingLag_ -= 1000.0 / 60.0988;
                const auto frame = emulator_.ppu().frame();
                audioBuffer_.clear();
                while (emulator_.ppu().frame() == frame) {
                    emulator_.step();
                    audioSampleCounter_ += 1.0;
                    if (audioSampleCounter_ >= audioCycles) {
                        audioSampleCounter_ -= audioCycles;
                        audioBuffer_.push_back(emulator_.apu().outputBuffer());
                    }
                }
                SDL_PutAudioStreamData(audioStream_, audioBuffer_.data(), audioBuffer_.size() * sizeof(float));
            }
        } else {
            perfCounter_ = SDL_GetPerformanceCounter();
        }
        SDL_UpdateTexture(texture_, nullptr, emulator_.ppu().frameBuffer().data(), 256 * sizeof(std::uint32_t));
        SDL_RenderClear(renderer_);
        SDL_RenderTexture(renderer_, texture_, nullptr, nullptr);
        if (state_ == EmulationState::Idle) {
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
        }
        SDL_RenderPresent(renderer_);
    }

    void SDLEmulator::openFileCallback(void *userdata, const char * const *files, [[maybe_unused]] int filter) {
        auto* self = static_cast<SDLEmulator*>(userdata);
        if (files && files[0]) {
            self->emulator_.loadROM(files[0]);
            self->state_ = EmulationState::Running;
            self->perfCounter_ = SDL_GetPerformanceCounter();
        }
    }
}
