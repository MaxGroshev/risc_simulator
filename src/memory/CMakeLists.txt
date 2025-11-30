add_library(memory memory.cpp mmu.cpp)

target_link_libraries(memory PUBLIC
    decoder
)