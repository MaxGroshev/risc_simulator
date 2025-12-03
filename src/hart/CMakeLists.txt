set(HART_SRC 
    hart.cpp)

set(JIT_SRC 
    jit/jit_instruction_factory.cpp
    jit/block_cache.cpp)

add_library(hart 
            ${HART_SRC}
            ${JIT_SRC})

target_link_libraries(hart PUBLIC
    decoder
    executer
)