add_library(memory memory.cpp)

target_include_directories(hart PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)