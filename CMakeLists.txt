# SPDX-License-Identifier: LGPL-3.0-or-later

# solinject - C++ Dependency Injection header-only library
# Copyright (C) 2022  SemperSolus0x3d
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


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

if (PROJECT_IS_TOP_LEVEL)
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
        FILE "${PROJECT_NAME}Config.cmake"
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
endif()
