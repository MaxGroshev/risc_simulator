# This is tmp main cmake it will be chaged by build.sh and main will be removed
cmake_minimum_required(VERSION 3.21)

#-----------------------------------------------------------------------------------------

project(decode_execute)

#-----------------------------------------------------------------------------------------

set(CMAKE_BUILD_TYPE "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS  -fsanitize=address)
    add_compile_definitions(MY_DEBUG)
    message("Debug_version")
endif ()

#-----------------------------------------------------------------------------------------


add_executable(${PROJECT_NAME}
    main.cpp
)

add_subdirectory(decode_execute_module/)
target_link_libraries(${PROJECT_NAME} 
    decoder
)

target_include_directories(${PROJECT_NAME}  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_execute_module
)

unset(TESTING_ENABLED CACHE)
unset(RUN_SET CACHE)