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
#include "solinject/Defines.hpp"
#include "IServiceTyped.hpp"
#include "solinject/exceptions/CircularDependencyException.hpp"

namespace sol::di::impl
{
    /** 
     * @brief Base for the DI service classes
     * @tparam T service type
     */
    template <class T>
    class ServiceBase : public IServiceTyped<T>
    {
    public:
        /// Base of the ServiceBase class
        using Base = IServiceTyped<T>;

        /// @copydoc sol::di::impl::IServiceTyped<T>::ServicePtr
        using ServicePtr = typename Base::ServicePtr;

        /// @copydoc sol::di::impl::IServiceTyped<T>::Factory
        using Factory = typename Base::Factory;

        /// @copydoc sol::di::impl::IService::Container
        using Container = typename Base::Container;

        virtual ~ServiceBase() = 0;

        /**
         * @brief Resolves the service and checks for circular dependencies
         * @param[in] container DI container
         * @returns pointer to a service instance
         * @throws sol::di::exc::CircularDependencyException
         */
        virtual ServicePtr GetService(const Container& container)
        {
            solinject_assert(!m_IsLocked && "There are no circular dependencies");

            if (m_IsLocked)
            {
                m_IsLocked = false;
                throw exc::CircularDependencyException(typeid(T));
            }

            m_IsLocked = true;
            auto service = this->GetServiceAsVoidPtr(container);
            m_IsLocked = false;

            solinject_req_assert(service != nullptr);

            return std::static_pointer_cast<T>(service);
        }

    protected:
        /**
         * @brief Field that indicates if the DI service is "locked"
         * 
         * When a DI service resolves a service, it becomes "locked"
         * until an instance of the service is returned.
         * If the DI service is already locked, it means 
         * that we have detected a circular dependency.
         */
        bool m_IsLocked = false;
    };

    template <class T>
    ServiceBase<T>::~ServiceBase() {}
}
