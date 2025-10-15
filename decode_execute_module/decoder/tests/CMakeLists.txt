cmake_minimum_required(VERSION 3.21)

project(decoder_tests)

set(UNIT_TESTS
    main.cpp
)

add_executable(${PROJECT_NAME} ${UNIT_TESTS})

# Линковка с gtest и decoder
target_link_libraries(${PROJECT_NAME} PRIVATE
    GTest::gtest_main
    decoder
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${gtest_SOURCE_DIR}/include 
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})