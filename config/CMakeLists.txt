cmake_minimum_required(VERSION 3.21)

#-----------------------------------------------------------------------------------------

project(config_parse)

#-----------------------------------------------------------------------------------------

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

set(FILE
    ./prog_config.hpp
    ./sim_config.hpp
    )

#-----------------------------------------------------------------------------------------

add_library(${PROJECT_NAME} INTERFACE  ${FILE})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} INTERFACE Boost::program_options )




