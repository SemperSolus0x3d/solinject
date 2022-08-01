cmake_minimum_required(VERSION 3.21)

project(solinject-tests VERSION 1.0 LANGUAGES CXX)

macro(add_test_executable name source_file)
    add_executable("${name}" "${source_file}")
    target_include_directories(
        "${name}"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/integration"
    )
    target_link_libraries("${name}" solinject)
    add_test(NAME "${name}" COMMAND "$<TARGET_FILE:${name}>")
endmacro()

macro(add_integration_test_executable name)
    add_test_executable("${name}" "integration/${name}.cpp")
    message(STATUS "${name} integration test added")
endmacro()

add_integration_test_executable("DIContainerTests")
