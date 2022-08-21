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

#include "IDIService.hpp"
#include "IDIServiceTyped.hpp"
#include "DIScopedServiceBuilder.hpp"

namespace sol::di::services
{
    /// Scoped DI service builders collection
    class DIScopedServiceBuilders
    {
    public:
        /**
         * @copydoc IDIServiceTyped<T>::Factory
         * @tparam T service type
         */
        template <class T>
        using Factory = typename IDIServiceTyped<T>::Factory;

        /**
         * @copydoc IDIServiceTyped<T>::ServicePtr
         * @tparam T service type
         */
        template <class T>
        using ServicePtr = typename IDIServiceTyped<T>::ServicePtr;

        /// Pointer to a @ref IDIService instance
        using DIServicePtr = std::shared_ptr<IDIService>;

        /// Pointer to a @ref IDIScopedServiceBuilder instance
        using DIScopedServiceBuilderPtr = std::shared_ptr<IDIScopedServiceBuilder>;
        
        /// Map of registered DI services
        using RegisteredServicesMap = std::map<std::type_index, std::vector<DIServicePtr>>;

        /// Map of registered DI service builders
        using RegisteredServiceBuildersMap = std::map<std::type_index, std::vector<DIScopedServiceBuilderPtr>>;

        /**
         * @brief Registers a scoped service
         * @tparam T service type
         * @param factory factory function
         */
        template<class T>
        void RegisterScopedService(const Factory<T> factory)
        {
            m_RegisteredServiceBuilders[std::type_index(typeid(T))].push_back(std::make_shared<DIScopedServiceBuilder<T>>(factory));
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
