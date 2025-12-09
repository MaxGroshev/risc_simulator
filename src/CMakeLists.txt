cmake_minimum_required(VERSION 3.21)

find_package(asmjit REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(decode_execute_module)
add_subdirectory(hart)
add_subdirectory(memory)
add_subdirectory(machine)

if(ENABLE_MODULES)
    add_subdirectory(modules)
else()
    message(STATUS "ENABLE_MODULES is OFF — skipping modules subdirectory")
endif()

target_link_libraries(${PROJECT_NAME} 
    machine
    asmjit::asmjit
    config_parse
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_execute_module
    ${ASMJIT_DIR} 
    ${CONFIG_DIRECTORY}
)