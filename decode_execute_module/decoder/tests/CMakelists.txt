cmake_minimum_required (VERSION 3.28)

#-----------------------------------------------------------------------------------------

project (decoder_tests)

#-----------------------------------------------------------------------------------------

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

#-----------------------------------------------------------------------------------------

#Unit_tests
set (UNIT_TESTS
    ./main.cpp)

#-----------------------------------------------------------------------------------------

add_executable            (${PROJECT_NAME} ${UNIT_TESTS})
target_include_directories(${PROJECT_NAME} PRIVATE ${GTEST_INCLUDE_DIRS} ../)
target_link_libraries     (${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES} decoder)


