// SPDX-License-Identifier: LGPL-3.0-or-later

/*
 * solinject - C++ Dependency Injection header-only library
 * Copyright (C) 2022  SemperSolus0x3d
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file
 * @brief Header file, which provides the
 * container itself and some other classes
 *
 * This header file provides the following classes:
 * - @ref sol::di::Container
 * - @ref sol::di::ContainerBuilder
 * - @ref sol::di::Configuration
 * - @ref sol::di::ConfigurationParser
 *
 * And the following exception classes:
 * - @ref sol::di::exc::DIException
 * - @ref sol::di::exc::CircularDependencyException
 * - @ref sol::di::exc::ServiceNotRegisteredException
 */

#pragma once

#include "solinject/Container.hpp"
#include "solinject/Configuration.hpp"
#include "solinject/ConfigurationParser.hpp"
#include "solinject/ContainerBuilder.hpp"
#include "solinject/Shortcuts.hpp"
