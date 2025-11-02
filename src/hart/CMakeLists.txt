add_library(hart hart.cpp)

target_link_libraries(hart PUBLIC
    decoder
    executer
)