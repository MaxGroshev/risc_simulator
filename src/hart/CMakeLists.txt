add_library(hart hart.cpp block_cache.cpp)

target_link_libraries(hart PUBLIC
    decoder
    executer
)

if(ENABLE_MODULES)
    target_link_libraries(hart PUBLIC modules)
else()
    message(STATUS "ENABLE_MODULES is OFF — hart will not link modules library")
endif()