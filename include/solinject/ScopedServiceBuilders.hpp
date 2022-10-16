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

#include <map>
#include <vector>
#include <algorithm>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <iterator>

#include "IService.hpp"
#include "IServiceTyped.hpp"
#include "ScopedServiceBuilder.hpp"

namespace sol::di::impl
{
    /// Scoped DI service builders collection
    class ScopedServiceBuilders
    {
    public:
        /**
         * @copydoc IServiceTyped<T>::Factory
         * @tparam T service type
         */
        template <class T>
        using Factory = typename IServiceTyped<T>::Factory;

        /**
         * @copydoc IServiceTyped<T>::ServicePtr
         * @tparam T service type
         */
        template <class T>
        using ServicePtr = typename IServiceTyped<T>::ServicePtr;

        /// Pointer to a @ref IService instance
        using DIServicePtr = std::shared_ptr<IService>;

        /// Pointer to a scoped service builder
        using ScopedServiceBuilderPtr = std::shared_ptr<IScopedServiceBuilder>;

        /// Map of registered DI services
        using RegisteredServicesMap = std::map<std::type_index, std::vector<DIServicePtr>>;

        /// Map of registered DI service builders
        using RegisteredServiceBuildersMap = std::map<std::type_index, std::vector<ScopedServiceBuilderPtr>>;

        /**
         * @brief Registers a scoped service
         * @tparam T service type
         * @param factory factory function
         */
        template<class T>
        void RegisterScopedService(Factory<T> factory)
        {
            RegisterScopedService(
                std::type_index(typeid(T)),
                std::make_shared<ScopedServiceBuilder<T>>(factory)
            );
        }

        /**
         * @brief Registers a scoped service
         * @param type service type
         * @param serviceBuilder service builder
         */
        void RegisterScopedService(std::type_index type, ScopedServiceBuilderPtr serviceBuilder)
        {
            m_RegisteredServiceBuilders[type].push_back(serviceBuilder);
        }

        /**
         * @brief Builds DI services
         * @returns registered services map
         */
        RegisteredServicesMap BuildDIServices() const
        {
            RegisteredServicesMap result;

            for (const auto& pair : m_RegisteredServiceBuilders)
            {
                auto& serviceBuilders = pair.second;

                std::vector<DIServicePtr> builtServices;
                builtServices.reserve(serviceBuilders.size());

                std::transform(
                    serviceBuilders.begin(),
                    serviceBuilders.end(),
                    std::back_inserter(builtServices),
                    [](auto& builder)
                    {
                        return builder->BuildDIService();
                    }
                );

                result[pair.first] = builtServices;
            }

            return result;
        }

    private:
        /// Registered service builders
        RegisteredServiceBuildersMap m_RegisteredServiceBuilders;
    };
}
