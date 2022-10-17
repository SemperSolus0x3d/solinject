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
#include <memory>
#include <functional>
#include "IService.hpp"

namespace sol::di::impl
{
    /// DI service interface
    template <class T>
    class IServiceTyped : public virtual IService
    {
    public:
        /// Pointer to an instance of a service
        using ServicePtr = typename std::shared_ptr<T>;

        /**
         * @brief Factory function that accepts a reference to 
         * a DI container and returns a pointer to an instance of a service
         */
        using Factory = typename std::function<ServicePtr(const Container&)>;

        virtual ~IServiceTyped() = 0;

        /**
         * @brief Resolves the service
         * @param[in] container DI container
         * @returns pointer to a service instance
         */
        virtual ServicePtr GetService(const Container& container) = 0;
    };

    template <class T>
    IServiceTyped<T>::~IServiceTyped() {}
}
