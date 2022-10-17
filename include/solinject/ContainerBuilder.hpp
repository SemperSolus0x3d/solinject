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
#include <variant>
#include <tuple>
#include <algorithm>
#include <typeinfo>
#include <typeindex>
#include "Container.hpp"
#include "Configuration.hpp"
#include "ConfigurationParser.hpp"
#include "Utils.hpp"

namespace sol::di
{
    /// @brief DI @ref Container builder
    class ContainerBuilder
    {
    public:
        /// @copydoc Container::Factory
        template <class T>
        using Factory = Container::Factory<T>;

        /// @copydoc ConfigurationItem::Key
        using Key = ConfigurationItem::Key;

        /// Pointer to an @ref impl::IService instance
        using DIServicePtr = std::shared_ptr<impl::IService>;

        /// @brief Registers an interface
        /// @tparam T the interface type
        /// @param key the interface key
        template <class T>
        void RegisterInterface(Key key)
        {
            m_RegisteredInterfaces.try_emplace(key, std::type_index(typeid(T)));
        }

        /// @brief Registers a service
        /// @tparam TService the service type
        /// @tparam ...TServiceParents the service parent types
        /// @param key the service key
        /// @param factory the service factory
        /// @see FACTORY
        template <class TService, class...TServiceParents>
        void RegisterService(Key key, Factory<TService> factory)
        {
            using namespace impl;

            LifetimeToServiceMap services;

            services[ServiceLifetime::Singleton] = std::make_shared<SingletonService<TService, TServiceParents...>>(factory);
            services[ServiceLifetime::Transient] = std::make_shared<TransientService<TService, TServiceParents...>>(factory);
            services[ServiceLifetime::Shared] = std::make_shared<SharedService<TService, TServiceParents...>>(factory);

            m_RegisteredServices[key] = std::move(services);
            m_RegisteredScopedServiceBuilders[key] =
                std::make_shared<ScopedServiceBuilder<TService, TServiceParents...>>(factory);

            m_RegisteredInterfaces.try_emplace(key, std::type_index(typeid(TService)));
        }

        /// @brief Builds a DI container
        /// @param[in] configuration the DI configuration
        /// @return built DI container
        /**
         * @warning Containers, built from the same ContainerBuilder
         * will share the same services. For example, they will share
         * the same instances of the services with `ServiceLifetime::Singleton` lifetime.
         */
        Container BuildContainer(const Configuration& configuration)
        {
            for (const auto& item : configuration.ConfigurationItems())
            {
                m_ServicesRegistration[item.InterfaceKey()].push_back(
                    std::make_tuple(item.ImplementationKey(), item.Lifetime())
                );
            }

            Container container;

            for (const auto& pair : m_ServicesRegistration)
            {
                auto interfaceKey = pair.first;

                std::type_index interfaceType =
                    m_RegisteredInterfaces.at(interfaceKey);

                for (const auto& item : pair.second)
                {
                    auto resolvedServices = ResolveService(interfaceKey, item);

                    for (const auto& service : resolvedServices)
                    {
                        if (std::holds_alternative<DIServicePtr>(service))
                            container.RegisterService(interfaceType, std::get<DIServicePtr>(service));
                        else
                            container.RegisterScopedServiceBuilder(
                                interfaceType,
                                std::get<ScopedServiceBuilderPtr>(service)
                            );
                    }
                }
            }

            m_ServicesRegistration.clear();

            return container;
        }
    private:
        using ScopedServiceBuilderPtr = std::shared_ptr<impl::IScopedServiceBuilder>;
        using LifetimeToServiceMap = std::map<ServiceLifetime, DIServicePtr>;
        using KeyToTypeMap = std::map<Key, std::type_index>;
        using RegisteredServicesMap = std::map<Key, LifetimeToServiceMap>;
        using RegisteredScopedServiceBuildersMap = std::map<Key, ScopedServiceBuilderPtr>;
        using KeyAndLifetime = std::tuple<Key, ServiceLifetime>;
        using ServiceKeyToImplementationKeyMap = std::map<Key, std::vector<KeyAndLifetime>>;

        ServiceKeyToImplementationKeyMap m_ServicesRegistration;
        RegisteredServicesMap m_RegisteredServices;
        RegisteredScopedServiceBuildersMap m_RegisteredScopedServiceBuilders;
        KeyToTypeMap m_RegisteredInterfaces;

        using ServiceOrBuilder = std::variant<DIServicePtr, ScopedServiceBuilderPtr>;

        std::vector<ServiceOrBuilder> ResolveService(Key interfaceKey, const KeyAndLifetime& item)
        {
            std::vector<ServiceOrBuilder> result;

            const auto& key = std::get<0>(item);
            const auto& lifetime = std::get<1>(item);

            if (interfaceKey != key)
                if (auto it = m_ServicesRegistration.find(key); 
                    it != m_ServicesRegistration.end())
                {
                    for (const auto& item2 : it->second)
                        impl::ConcatenateVectors(
                            result,
                            ResolveService(it->first, item2)
                        );
                }

            switch (lifetime)
            {
                case ServiceLifetime::None: break;
                case ServiceLifetime::Scoped:
                    if (auto it = m_RegisteredScopedServiceBuilders.find(key); 
                        it != m_RegisteredScopedServiceBuilders.end())
                    {
                        const auto& builder = it->second;
                        result.push_back(builder);
                    }
                default:
                    if (auto it = m_RegisteredServices.find(key); 
                        it != m_RegisteredServices.end())
                    {
                        auto& services = it->second;
                        result.push_back(services[lifetime]);
                    }
            }

            return result;
        }
    };
}
