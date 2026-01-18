cmake_minimum_required(VERSION 3.21)

project(riscv_sim)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(ASMJIT_DIR "third_party/asmjit")

option(DEBUG_EXECUTION "Enable debug execution logging" OFF)

option(ENABLE_MODULES "Turn on modules (will slow down simulation)" OFF)

option(PROFILING "Enable profiling with debug symbols (adds -g and frame pointers)" OFF)

option(GENERATE_PGO "Generate PGO instrumentation" OFF)
option(OPTIMIZE_PGO "Optimize with PGO profiles" OFF)

if(GENERATE_PGO AND OPTIMIZE_PGO)
    message(FATAL_ERROR "GENERATE_PGO and OPTIMIZE_PGO are mutually exclusive! Choose only one (you can use -U to unset).")
endif()

if(GENERATE_PGO)
    add_compile_options(-fprofile-generate)
    add_link_options(-fprofile-generate)
    message(STATUS "PGO: Instrumentation enabled (generate profiles)")
endif()

if(OPTIMIZE_PGO)
    add_compile_options(-fprofile-use -fprofile-correction)
    add_link_options(-fprofile-use)
    message(STATUS "PGO: Using profiles for optimization")
endif()

set (CMAKE_CXX_STANDARD 17)

# enable_testing()

# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
# FetchContent_MakeAvailable(googletest)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
    add_link_options(-fno-omit-frame-pointer -fsanitize=address)
    add_compile_definitions(MY_DEBUG)
    message(STATUS "Building in Debug mode")
else()
    add_compile_options(-O3 -flto -march=native -fno-math-errno -ffast-math)
endif ()

if (DEBUG_EXECUTION)
    add_compile_definitions(DEBUG_EXECUTION)
    message(STATUS "DEBUG_EXECUTION enabled via CMake option")
endif()

if (ENABLE_MODULES)
    add_compile_definitions(ENABLE_MODULES)
    message(STATUS "ENABLE_MODULES enabled via CMake option")
endif()

if (PROFILING)
    add_compile_options(-g -fno-omit-frame-pointer)
    add_link_options(-g -fno-omit-frame-pointer)
    message(STATUS "PROFILING enabled: Adding debug symbols (-g) and frame pointers for better stack traces")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(config)
add_subdirectory(src)