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
 * @brief File, which contains documentation for
 * feature-controlling defines.
 * 
 * This file contains documentation for macros,
 * which may be `#define`d by user code to enable or
 * disable some solinject's features (e.g. thread safety).
 */

#ifdef DOXYGEN
/**
 * @brief Macro, which, when defined, indicates that solinject
 * is being linked to a single-threaded program and, therefore,
 * its thread safety measures, such as mutex locks, may be disabled
 * for performance reasons.
 */
#define SOLINJECT_NOTHREADSAFE

/**
 * @brief Macro, which, when defined, indicates that solinject
 * is being linked to a tests project.
 * @warning This macro is intended for internal use only.
 */
#define SOLINJECT_TESTING
#endif
