cmake_minimum_required(VERSION 3.21)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(decode_execute_module)
add_subdirectory(hart)
add_subdirectory(memory)
add_subdirectory(machine)
add_subdirectory(modules)

target_link_libraries(${PROJECT_NAME} 
   machine
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_execute_module
)