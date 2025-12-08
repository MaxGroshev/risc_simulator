cmake_minimum_required(VERSION 3.21)

project(decode_execute)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

option(DEBUG_EXECUTION "Enable debug execution logging" OFF)

option(ENABLE_MODULES "Turn on modules (will slow down simulation)" OFF)

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
    add_compile_options(-flto)
endif ()

if (DEBUG_EXECUTION)
    add_compile_definitions(DEBUG_EXECUTION)
    message(STATUS "DEBUG_EXECUTION enabled via CMake option")
endif()

if (ENABLE_MODULES)
    add_compile_definitions(ENABLE_MODULES)
    message(STATUS "ENABLE_MODULES enabled via CMake option")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)