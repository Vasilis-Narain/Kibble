include(FetchContent)

# CppUTest is downloaded and built by CMake, nothing to install
FetchContent_Declare(cpputest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG        v4.0
    GIT_SHALLOW    TRUE
)
set(TESTS OFF CACHE BOOL "Don't build CppUTest's own tests" FORCE)
# CppUTest v4.0 asks for an ancient CMake version, which CMake 4.x rejects
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(cpputest)

# Adds the unit tests for one module in src/<module>:
#   sources:  src/<module>/*.c + tests/<module>/*.cpp
#   runs as:  ctest test named <module>
function(add_module_test module)
    file(GLOB module_src ${PROJECT_SOURCE_DIR}/src/${module}/*.c)
    file(GLOB test_src ${CMAKE_CURRENT_SOURCE_DIR}/${module}/*.cpp)

    add_executable(${module}_tests ${module_src} ${test_src})
    target_include_directories(${module}_tests PRIVATE ${PROJECT_SOURCE_DIR}/src/${module})
    target_link_libraries(${module}_tests PRIVATE CppUTest CppUTestExt)

    add_test(NAME ${module} COMMAND ${module}_tests)
endfunction()

add_module_test(average)
