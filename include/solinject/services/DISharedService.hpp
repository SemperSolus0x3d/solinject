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
     * @brief Shared DI service
     * @tparam T service type
     */
    template<class T>
    class DISharedService : public DIServiceBase<T>
    {
    public:
        /// Base of the @ref DISharedService class
        using Base = DIServiceBase<T>;

        /// @copydoc DIServiceBase<T>::Container
        using Container = typename Base::Container;

        /// @copydoc DIServiceBase<T>::ServicePtr
        using ServicePtr = typename Base::ServicePtr;

        /// @copydoc DIServiceBase<T>::Factory
        using Factory = typename Base::Factory;

        /// @ref std::weak_ptr to a service instance
        using ServiceWeakPtr = std::weak_ptr<T>;

        /**
         * @brief Constructor
         * @param factory the factory function
         */
        DISharedService(const Factory factory) : m_Factory(factory), m_ServicePtr()
        {
        }

    protected:
        ServicePtr GetServiceInternal(const Container& container) override
        {
            ServicePtr instancePtr = m_ServicePtr.lock();

            if (instancePtr != nullptr)
            {
                return instancePtr;
            }
            else
            {
                ServicePtr newInstancePtr = m_Factory(container);

                solinject_req_assert(newInstancePtr != nullptr && "Factory should never return nullptr");

                m_ServicePtr = ServiceWeakPtr(newInstancePtr);
                return newInstancePtr;
            }
        }

    private:
        /// Pointer to the service instance
        ServiceWeakPtr m_ServicePtr;

        /// Factory function
        Factory m_Factory;
    }; // class DISharedService
} // sol::di::services
