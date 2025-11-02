project(decode_execute_module)

set(RUBY_GEN_SCRIPT ${CMAKE_SOURCE_DIR}/isa/dsl/main.rb) 

set(GENERATED_DECODER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/decoder)
set(GENERATED_EXECUTER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/executer)

set(GENERATED_ENUM_HPP ${CMAKE_SOURCE_DIR}/instruction_opcodes_gen.hpp)
set(GENERATED_DECODER_CPP ${GENERATED_DECODER_DIR}/rv32i_decoder_gen.cpp)
set(GENERATED_DECODER_HPP ${GENERATED_DECODER_DIR}/rv32i_decoder_gen.hpp)
set(GENERATED_EXECUTER_CPP ${GENERATED_EXECUTER_DIR}/rv32i_executer_gen.cpp)
set(GENERATED_EXECUTER_HPP ${GENERATED_EXECUTER_DIR}/rv32i_executer_gen.hpp)

# Generating Ruby code
add_custom_command(
    OUTPUT ${GENERATED_ENUM_HPP} ${GENERATED_DECODER_CPP} ${GENERATED_DECODER_HPP} ${GENERATED_EXECUTER_CPP} ${GENERATED_EXECUTER_HPP}
    COMMAND ruby ${RUBY_GEN_SCRIPT}
    DEPENDS ${RUBY_GEN_SCRIPT}  
            ${CMAKE_SOURCE_DIR}/isa/rv32i_isa.yml  
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/isa/ 
    COMMENT "Generating RV32I decoder and executer files using Ruby"
    VERBATIM
)

# Compile after generating in Ruby
add_custom_target(run_ruby_generation DEPENDS ${GENERATED_DECODER_CPP} ${GENERATED_DECODER_HPP} ${GENERATED_EXECUTER_CPP} ${GENERATED_EXECUTER_HPP})

add_subdirectory(decoder)
add_subdirectory(executer)