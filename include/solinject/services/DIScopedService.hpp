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
#include "DISingletonService.hpp"

namespace sol::di::services
{
    template<class T>
    class DIScopedService : public DISingletonService<T>
    {
    public:
        using Base = DIServiceBase<T>;
        using Factory = typename Base::Factory;

        DIScopedService(const Factory factory) : DISingletonService<T>(factory)
        {
        }
    }; // class DISingletonService
} // sol::di::services
