cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Must match executable name: CubeMX's file adds its sources to ${CMAKE_PROJECT_NAME}
project(handheld C)

# One project, two builds, picked by the preset:
#   Debug / Release -> arm toolchain file -> firmware (handheld.elf)
#   host-tests      -> host compiler      -> unit tests (ctest)
if(CMAKE_CROSSCOMPILING)
    enable_language(ASM)

    add_executable(handheld)

    # CubeMX part: HAL, startup, Core/, defines, include paths
    add_subdirectory(cubemx/cmake/stm32cubemx)

    # Own code
    target_sources(handheld PRIVATE
        src/app.c
        # more files here
        src/input/input.c
        src/input/input_hw.c
    )

    target_include_directories(handheld PRIVATE
        src
        src/input
    )

    target_link_libraries(handheld
        stm32cubemx
    )
else()
    enable_language(CXX)
    enable_testing()
    add_subdirectory(tests)
endif()
