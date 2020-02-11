cmake_minimum_required(VERSION 4.0)
project(SpikyNES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(SPIKY_NES_ENABLE_TESTS "Build tests" OFF)
option(SPIKY_NES_ENABLE_ROM_TESTS "Build ROM tests" OFF)
option(SPIKY_NES_ENABLE_STRICT "Use strict compile options" OFF)
option(SPIKY_NES_ENABLE_COVERAGE "Enable coverage reporting" OFF)

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

add_library(SpikyNESCore STATIC
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
)

target_include_directories(SpikyNESCore
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

    add_executable(runTests
            tests/test_main.cpp
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
            tests/core/ppu/test_registers.cpp
            tests/core/test_utils.cpp
            tests/core/ppu/test_frame_timing.cpp
            tests/core/ppu/test_shift_registers.cpp
            tests/core/ppu/test_scroll.cpp
            tests/core/ppu/test_background_rendering.cpp
            tests/core/ppu/test_sprite_rendering.cpp
            tests/core/ppu/test_ppu_utils.h
            tests/core/ppu/test_ppu_utils.cpp
            tests/core/ppu/rom/test_sprite_hit_tests.cpp
    )

    if(SPIKY_NES_ENABLE_ROM_TESTS)
        target_sources(runTests PRIVATE
                tests/core/cpu/rom/test_nestest.cpp
                tests/core/ppu/rom/test_ppu_vbl_nmi.cpp
                tests/core/ppu/rom/test_ppu_tests.cpp
                tests/core/ppu/rom/test_sprite_hit_tests.cpp
        )
        target_compile_definitions(runTests PRIVATE SPIKY_NES_ROM_TESTS_ENABLED)
    endif()

    target_link_libraries(runTests
            PRIVATE SpikyNESCore
            PRIVATE gtest gtest_main
    )

    add_test(NAME SpikyNESCoreTests COMMAND runTests)
endif()
