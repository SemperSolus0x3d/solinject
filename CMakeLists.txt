cmake_minimum_required(VERSION 3.24)

project(solinject VERSION 1.0 LANGUAGES CXX)

get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)

option(ENABLE_TESTS "Enable tests" ON)

add_library(${CMAKE_PROJECT_NAME} INTERFACE)

target_compile_features(${CMAKE_PROJECT_NAME} INTERFACE cxx_std_20)

target_include_directories(
    ${CMAKE_PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(ENABLE_TESTS AND NOT IS_SUBPROJECT)
    enable_testing()
    add_subdirectory(test)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    TARGETS ${CMAKE_PROJECT_NAME}
    EXPORT ${CMAKE_PROJECT_NAME}
)

install(
    DIRECTORY include/ 
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}"
)

# Export project and install config
install(
    EXPORT ${CMAKE_PROJECT_NAME}
    FILE "${PROJECT_NAME}Config.cmake"
    NAMESPACE sol::
    DESTINATION cmake
)

# Create package version file
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Install package version file
install(
    FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" 
    DESTINATION cmake
)
