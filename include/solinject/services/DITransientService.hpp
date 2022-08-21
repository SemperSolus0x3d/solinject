﻿// SPDX-License-Identifier: LGPL-3.0-or-later

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
#include "solinject/Defines.hpp"
#include "DIServiceBase.hpp"

namespace sol::di::services
{
    /**
     * @brief Transient DI service
     * @tparam T service type
     */
    template<class T>
    class DITransientService : public DIServiceBase<T>
    {
    public:
        /// Base of the @ref DITransientService class
        using Base = DIServiceBase<T>;

        /// @copydoc DIServiceBase<T>::Container
        using Container = typename Base::Container;

        /// @copydoc DIServiceBase<T>::ServicePtr
        using ServicePtr = typename Base::ServicePtr;

        /// @copydoc DIServiceBase<T>::Factory
        using Factory = typename Base::Factory;

        /**
         * @brief Constructor
         * @param factory factory function
         */
        DITransientService(const Factory factory) : m_Factory(factory)
        {
        }

    protected:
        ServicePtr GetServiceInternal(const Container& container) override
        {
            auto service = m_Factory(container);

            solinject_req_assert(service != nullptr && "Factory should never return nullptr");

            return service;
        }

    private:
        /// Factory function
        Factory m_Factory;
    }; // class DITransientService
} // sol::di::services
