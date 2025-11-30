add_library(machine machine.cpp)

target_link_libraries(machine PUBLIC 
    hart 
    memory
    config_parse
)