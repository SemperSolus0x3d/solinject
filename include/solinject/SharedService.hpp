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
     * @brief Shared DI service
     * @tparam T service type
     */
    template<class TService, class...TServiceParents>
    class SharedService :
        public ServiceBase<TService>,
        public ServiceBase<TServiceParents>...
    {
        static_assert(
            std::conjunction_v<std::is_base_of<TServiceParents, TService>...>,
            "The TServiceParents types must be derived from the TService type"
        );
    public:
        /// Base of the @ref SharedService class
        using Base = ServiceBase<TService>;

        /// @copydoc ServiceBase<TService>::Container
        using Container = typename Base::Container;

        /// @copydoc ServiceBase<TService>::ServicePtr
        using ServicePtr = typename Base::ServicePtr;

        /// @copydoc ServiceBase<TService>::Factory
        using Factory = typename Base::Factory;

        /// @ref std::weak_ptr to a service instance
        using ServiceWeakPtr = std::weak_ptr<TService>;

        /// @copydoc sol::di::impl::IService::VoidPtr
        using VoidPtr = typename IService::VoidPtr;

        /**
         * @brief Constructor
         * @param factory the factory function
         */
        SharedService(Factory factory) : m_Factory(factory), m_ServicePtr()
        {
        }

    protected:
        virtual VoidPtr GetServiceAsVoidPtr(const Container& container) override
        {
            ServicePtr instancePtr = m_ServicePtr.lock();

            if (instancePtr != nullptr)
            {
                return std::static_pointer_cast<void>(instancePtr);
            }
            else
            {
                ServicePtr newInstancePtr = m_Factory(container);

                solinject_req_assert(newInstancePtr != nullptr && "Factory should never return nullptr");

                m_ServicePtr = ServiceWeakPtr(newInstancePtr);
                return std::static_pointer_cast<void>(newInstancePtr);
            }
        }

    private:
        /// Pointer to the service instance
        ServiceWeakPtr m_ServicePtr;

        /// Factory function
        Factory m_Factory;
    }; // class SharedService
} // sol::di::impl
