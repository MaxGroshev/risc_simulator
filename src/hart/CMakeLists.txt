add_library(hart hart.cpp block_cache.cpp)

target_link_libraries(hart PUBLIC
    decoder
    executer
)