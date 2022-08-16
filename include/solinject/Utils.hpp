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
#include <mutex>
#include <type_traits>

namespace sol::di::utils
{
    class Empty
    {
    public:
        Empty() {}

        template<class...TArgs>
        Empty(const TArgs&...args) {}
    };

    template <class TMutex, bool isEnabled>
    using DiscardableMutex = std::conditional_t<isEnabled, TMutex, Empty>;

    template <class TMutex, bool isEnabled>
    using DiscardableLock = std::conditional_t<isEnabled, std::lock_guard<TMutex>, Empty>;

    template <bool isEnabled, class...TMutexes>
    using DiscardableScopedLock = std::conditional_t<isEnabled, std::scoped_lock<TMutexes...>, Empty>;
}
