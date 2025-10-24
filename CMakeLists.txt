cmake_minimum_required(VERSION 3.21)

project(decode_execute)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

set(CMAKE_BUILD_TYPE "Debug")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
    add_link_options(-fno-omit-frame-pointer -fsanitize=address)
    add_compile_definitions(MY_DEBUG)
    message(STATUS "Building in Debug mode")
endif ()

set(SOURCES
    src/main.cpp
    src/hart.cpp
    src/machine.cpp
    src/memory.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(decode_execute_module)

target_link_libraries(${PROJECT_NAME} decoder executer)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_execute_module
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)