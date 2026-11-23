cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Must match executable name: CubeMX's file adds its sources to ${CMAKE_PROJECT_NAME}
project(handheld C)

# FatFS: builds on both builds (test / debug/release)
add_library(fatfs STATIC 
    FatFs/ff.c
    FatFs/ffunicode.c
)
target_include_directories(fatfs SYSTEM PUBLIC FatFs)
#

# One project, two builds, picked by the preset:
#   Debug / Release -> arm toolchain file -> firmware (handheld.elf)
#   host-tests      -> host compiler      -> unit tests (ctest)
if(CMAKE_CROSSCOMPILING)
    enable_language(ASM)
    include(FetchContent)

    # FreeRTOS
    FetchContent_Declare(freertos_kernel
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG V11.3.3
        GIT_SHALLOW TRUE
    )


    add_executable(handheld)

    # CubeMX part: HAL, startup, Core/, defines, include paths
    add_subdirectory(cubemx/cmake/stm32cubemx)
    # 

    # FreeRTOS:
    add_library(freertos_config INTERFACE)
    target_include_directories(freertos_config SYSTEM INTERFACE freertos)
    set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "")
    set(FREERTOS_HEAP 4 CACHE STRING "")
    target_link_libraries(freertos_config INTERFACE stm32cubemx)
    FetchContent_MakeAvailable(freertos_kernel)
    #

    # Own code
    set(APP_SRC
        # App entry point
        src/app.c

        # Log system
        src/log/log.c

        # Input system
        src/input/input.c
        src/input/input_hw.c

        # Audio system
        src/audio/audio.c
        src/audio/audio_hw.c
        src/audio/audio_sfx.c
        src/audio/audio_voice.c

        # SD File System
        src/sd_fs/sd_diskio_hw.c
    )
    target_sources(handheld PRIVATE ${APP_SRC})
    set_source_files_properties(${APP_SRC} PROPERTIES COMPILE_OPTIONS "-Wextra")

    target_include_directories(handheld PRIVATE
        src
        src/input
        src/audio
        src/log
    )

    target_link_libraries(handheld
        stm32cubemx
        freertos_kernel
        fatfs
    )
else()
    enable_language(CXX)
    enable_testing()
    add_subdirectory(tests)
endif()

