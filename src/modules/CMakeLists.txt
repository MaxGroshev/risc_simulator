add_library(modules example_module.cpp)
target_include_directories(modules PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(modules PUBLIC
    config_parse
)