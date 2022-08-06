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

project(solinject-tests VERSION 1.0 LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

macro(add_test_executable name source_file)
    add_executable("${name}" "${source_file}")
    target_include_directories(
        "${name}"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/integration"
    )
    target_link_libraries("${name}" solinject Threads::Threads)
    add_test(NAME "${name}" COMMAND "$<TARGET_FILE:${name}>")
endmacro()

macro(add_integration_test_executable name)
    add_test_executable("${name}" "integration/${name}.cpp")
    message(STATUS "${name} integration test added")
endmacro()

add_integration_test_executable("DIContainerTests")
