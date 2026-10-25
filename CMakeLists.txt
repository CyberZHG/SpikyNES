cmake_minimum_required(VERSION 4.0)
project(SpikyNes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(SPIKY_NES_ENABLE_TESTS "Build tests" OFF)
option(SPIKY_NES_ENABLE_ROM_TESTS "Build ROM tests" OFF)
option(SPIKY_NES_ENABLE_STRICT "Use strict compile options" OFF)
option(SPIKY_NES_ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(SPIKY_NES_ENABLE_UI "Build SDL UI" OFF)
option(SPIKY_NES_BIND_PYTHON "Enable Python binding" OFF)
option(SPIKY_NES_BIND_ES "Enable ECMAScript binding" OFF)

if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

if(SPIKY_NES_ENABLE_STRICT)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        add_compile_options(
                -Wall
                -Wextra
                -Werror
                -Wpedantic
                -Wmissing-include-dirs
                -Wundef
                -Wredundant-decls
        )
    endif()
endif()

if(SPIKY_NES_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g -fno-elide-constructors -fno-default-inline)
        add_link_options(--coverage)
    endif()
endif()

add_library(SpikyNesCore STATIC
        include/core/emulator.h
        src/core/emulator.cpp
        include/core/cpu.h
        src/core/cpu.cpp
        include/core/ppu.h
        src/core/ppu.cpp
        include/core/apu.h
        src/core/apu.cpp
        include/core/bus.h
        src/core/bus.cpp
        include/core/cartridge.h
        src/core/cartridge.cpp
        include/core/constants.h
        include/core/utils.h
        src/core/utils.cpp
        src/core/cpu/addressing.cpp
        src/core/cpu/operation.cpp
        src/core/cpu/unofficial.cpp
        include/core/controller.h
        src/core/controller.cpp
        include/core/mapper.h
        src/core/mapper.cpp
        src/core/mapper.cpp
        include/core/mapper/mapper_000.h
        src/core/mapper/mapper_000.cpp
        include/core/mapper/mapper_003.h
        src/core/mapper/mapper_003.cpp
        include/core/mapper/mapper_001.h
        src/core/mapper/mapper_001.cpp
        include/core/mapper/mapper_002.h
        src/core/mapper/mapper_002.cpp
        include/core/mapper/mapper_004.h
        src/core/mapper/mapper_004.cpp
)

target_include_directories(SpikyNesCore
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(SPIKY_NES_ENABLE_TESTS)
    enable_testing()

    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
    )

    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(
            tinypng
            GIT_REPOSITORY https://github.com/CyberZHG/TinyPNG.git
            GIT_TAG v1.0.0
    )

    FetchContent_MakeAvailable(tinypng)

    add_executable(runTests
            tests/test_main.cpp
            tests/core/test_image_utils.h
            tests/core/test_image_utils.cpp
            tests/core/cpu/test_cpu_utils.cpp
            tests/core/cpu/test_cpu_utils.h
            tests/core/cpu/test_load.cpp
            tests/core/cpu/test_store.cpp
            tests/core/cpu/test_transfer.cpp
            tests/core/cpu/test_stack.cpp
            tests/core/cpu/test_shift.cpp
            tests/core/cpu/test_logical.cpp
            tests/core/cpu/test_arithmetic.cpp
            tests/core/cpu/test_increment.cpp
            tests/core/cpu/test_flags.cpp
            tests/core/cpu/test_jump.cpp
            tests/core/cpu/test_control.cpp
            tests/core/cpu/test_branch.cpp
            tests/core/cpu/test_nop.cpp
            tests/core/test_cartridge.cpp
            tests/core/test_controller.cpp
            tests/core/ppu/test_registers.cpp
            tests/core/test_utils.cpp
            tests/core/ppu/test_frame_timing.cpp
            tests/core/ppu/test_shift_registers.cpp
            tests/core/ppu/test_scroll.cpp
            tests/core/ppu/test_background_rendering.cpp
            tests/core/ppu/test_sprite_rendering.cpp
            tests/core/ppu/test_ppu_utils.h
            tests/core/ppu/test_ppu_utils.cpp
            tests/core/mapper/test_mapper_000.cpp
            tests/core/mapper/test_mapper_001.cpp
            tests/core/mapper/test_mapper_002.cpp
            tests/core/mapper/test_mapper_003.cpp
            tests/core/mapper/test_mapper_004.cpp
            tests/core/apu/test_pulse.cpp
            tests/core/apu/test_triangle.cpp
            tests/core/apu/test_noise.cpp
            tests/core/apu/test_dmc.cpp
            tests/core/ppu/rom/test_ppu_read_buffer.cpp
            tests/core/ppu/rom/test_ppu_open_bus.cpp
    )

    if(SPIKY_NES_ENABLE_ROM_TESTS)
        target_sources(runTests PRIVATE
                tests/core/cpu/rom/test_nestest.cpp
                tests/core/cpu/rom/test_cpu_timing.cpp
                tests/core/cpu/rom/test_all_instrs_v5.cpp
                tests/core/ppu/rom/test_ppu_tests.cpp
                tests/core/ppu/rom/test_sprite_hit_tests.cpp
                tests/core/ppu/rom/test_sprite_overflow_tests.cpp
                tests/core/ppu/rom/test_vbl_nmi_timing.cpp
                tests/core/apu/rom/test_apu_test.cpp
                tests/core/mapper/rom/test_mmc3.cpp
        )
        target_compile_definitions(runTests PRIVATE SPIKY_NES_ROM_TESTS_ENABLED)
    endif()

    target_link_libraries(runTests
            PRIVATE SpikyNesCore
            PRIVATE gtest gtest_main
            PRIVATE TinyPNG
    )

    add_test(NAME SpikyNesCoreTests COMMAND runTests)
endif()

if(SPIKY_NES_ENABLE_UI)
    include(FetchContent)

    FetchContent_Declare(
            SDL
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.4.0
    )

    FetchContent_MakeAvailable(SDL)

    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG docking
    )
    FetchContent_MakeAvailable(imgui)

    add_executable(SpikyNesUI
            src/ui/main.cpp
            include/ui/sdl_emulator.h
            src/ui/sdl_emulator.cpp
    )

    target_sources(SpikyNesUI PRIVATE
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    )

    target_include_directories(SpikyNesUI
            PRIVATE ${imgui_SOURCE_DIR}
            PRIVATE ${imgui_SOURCE_DIR}/backends
            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(SpikyNesUI
            PRIVATE SpikyNesCore SDL3::SDL3
    )
endif()

if(SPIKY_NES_BIND_PYTHON)
    include(FetchContent)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG v3.0
    )
    FetchContent_MakeAvailable(pybind11)

    pybind11_add_module(_core
            python/binding.cpp
    )
    target_link_libraries(_core
            PRIVATE SpikyNesCore
    )
    install(TARGETS _core
            LIBRARY DESTINATION spiky_nes)
    install(FILES python/wrapper/__init__.py DESTINATION spiky_nes)
endif()

if(SPIKY_NES_BIND_ES)
    add_executable(SpikyNesWASM
            wasm/binding.cpp
    )

    target_link_libraries(SpikyNesWASM
            PRIVATE SpikyNesCore
    )

    target_compile_options(SpikyNesWASM PRIVATE "-O3")

    set_target_properties(SpikyNesWASM PROPERTIES SUFFIX ".js")

    target_link_options(SpikyNesWASM PRIVATE
            "--bind"
            "-sMODULARIZE=1"
            "-sEXPORT_ES6=1"
            "-sEXPORT_NAME=SpikyNesWASMModule"
            "-sALLOW_MEMORY_GROWTH=1"
    )
endif()
