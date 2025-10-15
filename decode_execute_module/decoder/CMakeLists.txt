project(decoder)

set(SOURCE
    rv32i_decoder_gen.cpp
)

add_library(${PROJECT_NAME} ${SOURCE})
add_dependencies(${PROJECT_NAME} run_ruby_generation)  # Зависимость от генерации

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)