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

/// @file

#pragma once
#include "SingletonService.hpp"

namespace sol::di::impl
{
    /**
     * @brief Scoped DI service
     * @tparam T service type
     */
    template<class TService, class...TServiceParents>
    class ScopedService :
        public SingletonService<TService, TServiceParents...>
    {
        static_assert(
            std::conjunction_v<std::is_base_of<TServiceParents, TService>...>,
            "The TServiceParents types must be derived from the TService type"
        );
    public:
        /// Base of the @ref ScopedService class
        using Base = SingletonService<TService, TServiceParents...>;

        /// @copydoc SingletonService<T>::Factory
        using Factory = typename Base::Factory;

        /// @copydoc sol::di::impl::IService::VoidPtr
        using VoidPtr = typename IService::VoidPtr;

        /**
         * @brief Constructor
         * @param factory factory function
         */
        ScopedService(Factory factory) : Base(factory)
        {
        }
    }; // class SingletonService
} // sol::di::impl
