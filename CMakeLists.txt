cmake_minimum_required(VERSION 3.21)

project(solinject VERSION 1.0.0 LANGUAGES CXX)

set(PROJECT_NAMESPACE sol::)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

add_library(${PROJECT_NAME} INTERFACE)
add_library("${PROJECT_NAMESPACE}${PROJECT_NAME}" ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(
    ${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(test)
endif()

# Create package version file
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
)

# Export project and install config
install(
    EXPORT ${PROJECT_NAME}
    FILE "${PROJECT_NAME}_Config.cmake"
    NAMESPACE ${PROJECT_NAMESPACE}
    DESTINATION cmake
)

# Install includes
install(
    DIRECTORY include/
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)

# Install package version file
install(
    FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" 
    DESTINATION cmake
)
