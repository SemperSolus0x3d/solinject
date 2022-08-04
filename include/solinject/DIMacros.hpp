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

#pragma once

#define FROM_DI(class_) (c.template GetRequiredServicePtr<class_>())

#define RegisterSingletonService(container, class_, ...) \
    (container).template RegisterSingletonService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterSingletonInterface(container, interface_, implementation, ...) \
    (container).template RegisterSingletonService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })

#define RegisterTransientService(container, class_, ...) \
    (container).template RegisterTransientService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterTransientInterface(container, interface_, implementation, ...) \
    (container).template RegisterTransientService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })

#define RegisterSharedService(container, class_, ...) \
    (container).template RegisterSharedService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterSharedInterface(container, interface_, implementation, ...) \
    (container).template RegisterSharedService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })
