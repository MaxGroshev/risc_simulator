set(HART_SRC 
    hart.cpp)

set(JIT_SRC 
    jit/naive_cache.cpp)

add_library(hart 
            ${HART_SRC}
            ${JIT_SRC}
)
            

target_link_libraries(hart PUBLIC
    decoder
    executer
    config_parse
)

if(ENABLE_MODULES)
    target_link_libraries(hart PUBLIC modules)
else()
    message(STATUS "ENABLE_MODULES is OFF — hart will not link modules library")
endif()