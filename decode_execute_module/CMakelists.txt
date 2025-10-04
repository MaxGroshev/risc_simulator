find_package(Ruby REQUIRED)

add_custom_target(
    run_ruby_generation
    COMMAND ${RUBY_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../isa_dsl/main.rb
    # WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Executing Ruby build scripts"
)

add_subdirectory(decoder)
add_subdirectory(executer)
