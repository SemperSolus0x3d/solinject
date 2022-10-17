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
#include "ServiceBase.hpp"

namespace sol::di::impl
{
    /**
     * @brief Singleton DI service
     * @tparam T service type
     */
    template<class TService, class...TServiceParents>
    class SingletonService : 
        public ServiceBase<TService>,
        public ServiceBase<TServiceParents>...
    {
        static_assert(
            std::conjunction_v<std::is_base_of<TServiceParents, TService>...>,
            "The TServiceParents types must be derived from the TService type"
        );
    public:
        /// Base of the @ref SingletonService class
        using Base = ServiceBase<TService>;

        /// @copydoc sol::di::impl::ServiceBase<T>::Container
        using Container = typename Base::Container;

        /// @copydoc sol::di::impl::ServiceBase<T>::ServicePtr
        using ServicePtr = typename Base::ServicePtr;

        /// @copydoc sol::di::impl::ServiceBase<T>::Factory
        using Factory = typename Base::Factory;

        /// @copydoc sol::di::impl::IService::VoidPtr
        using VoidPtr = typename IService::VoidPtr;

        /**
         * @brief Constructor
         * @param service pointer to a service instance
         */
        SingletonService(ServicePtr service) : m_ServicePtr(service)
        {
        }

        /**
         * @brief Constructor
         * @param factory factory function
         */
        SingletonService(Factory factory) : m_Factory(factory), m_ServicePtr(nullptr)
        {
        }

        virtual ~SingletonService() {}

    protected:
        /// @copydoc sol::di::impl::IService::GetServiceAsVoidPtr
        virtual VoidPtr GetServiceAsVoidPtr(const Container& container) override
        {
            if (m_ServicePtr == nullptr)
            {
                m_ServicePtr = m_Factory(container);
                solinject_req_assert(m_ServicePtr != nullptr && "Factory should never return nullptr");
            }

            return std::static_pointer_cast<void>(m_ServicePtr);
        }

    private:
        /// Pointer to the service instance
        ServicePtr m_ServicePtr;

        /// Factory function
        Factory m_Factory;
    }; // class SingletonService
} // sol::di::impl
