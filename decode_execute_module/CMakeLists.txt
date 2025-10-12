project(decode_execute_module)

set(RUBY_GEN_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/../isa_dsl/main.rb) 
set(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/decoder)
set(GENERATED_CPP ${GENERATED_DIR}/rv32i_decoder_gen.cpp)
set(GENERATED_HPP ${GENERATED_DIR}/rv32i_decoder_gen.hpp)

# Кастомная команда для генерации файлов через Ruby
add_custom_command(
    OUTPUT ${GENERATED_CPP} ${GENERATED_HPP}
    COMMAND ruby ${RUBY_GEN_SCRIPT}
    DEPENDS ${RUBY_GEN_SCRIPT}  
            ${CMAKE_CURRENT_SOURCE_DIR}/../isas/rv32i_isa.yml  
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../isa_dsl  
    COMMENT "Generating RV32I decoder files using Ruby"
    VERBATIM
)

# Кастомная цель для генерации
add_custom_target(run_ruby_generation DEPENDS ${GENERATED_CPP} ${GENERATED_HPP})

add_subdirectory(decoder)
