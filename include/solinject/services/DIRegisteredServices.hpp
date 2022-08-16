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

#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include <memory>
#include <typeinfo>
#include <typeindex>

#include "IDIService.hpp"
#include "IDIServiceTyped.hpp"
#include "DISingletonService.hpp"
#include "DITransientService.hpp"
#include "DISharedService.hpp"
#include "DIScopedService.hpp"
#include "solinject/exceptions/ServiceNotRegisteredException.hpp"

namespace sol::di::services
{
    class DIRegisteredServices
    {
    public:
        template <class T>
        using Factory = typename IDIServiceTyped<T>::Factory;

        template <class T>
        using ServicePtr = typename IDIServiceTyped<T>::ServicePtr;

        using DIServicePtr = std::shared_ptr<IDIService>;
        using RegisteredServicesMap = std::map<std::type_index, std::vector<DIServicePtr>>;

        DIRegisteredServices() {}

        DIRegisteredServices(RegisteredServicesMap&& services) :
            m_RegisteredServices(std::move(services))
        {
        }

        DIRegisteredServices(const DIRegisteredServices& other) :
            m_RegisteredServices(other.m_RegisteredServices)
        {
        }

        DIRegisteredServices(DIRegisteredServices&& other) : DIRegisteredServices()
        {
            swap(*this, other);
        }

        DIRegisteredServices& operator=(DIRegisteredServices other)
        {
            swap(*this, other);
            return *this;
        }

        DIRegisteredServices& operator=(DIRegisteredServices&& other)
        {
            swap(*this, other);
            return *this;
        }

        friend void swap(DIRegisteredServices& a, DIRegisteredServices& b)
        {
            using std::swap;

            swap(a.m_RegisteredServices, b.m_RegisteredServices);
        }

        void Merge(DIRegisteredServices&& other)
        {
            for (const auto& pair : other.m_RegisteredServices)
            {
                auto& source = pair.second;
                auto& destination = m_RegisteredServices[pair.first];

                destination.insert(
                    destination.end(),
                    std::make_move_iterator(source.begin()),
                    std::make_move_iterator(source.end())
                );
            }
        }

        template<class T>
        void RegisterSingletonService(const Factory<T> factory)
        {
            RegisterServiceInternal<T, DISingletonService<T>>(factory);
        }

        template<class T>
        void RegisterSingletonService(ServicePtr<T> instance)
        {
            RegisterServiceInternal<T, DISingletonService<T>>(instance);
        }

        template<class T>
        void RegisterTransientService(const Factory<T> factory)
        {
            RegisterServiceInternal<T, DITransientService<T>>(factory);
        }

        template<class T>
        void RegisterSharedService(const Factory<T> factory)
        {
            RegisterServiceInternal<T, DISharedService<T>>(factory);
        }

        template<class T>
        ServicePtr<T> GetRequiredService(const DIContainer& container) const
        {
            return GetServiceInternal<T, false>(container);
        }

        template <class T>
        ServicePtr<T> GetService(const DIContainer& container) const
        {
            return GetServiceInternal<T, true>(container);
        }

        template <class T>
        std::vector<ServicePtr<T>> GetServices(const DIContainer& container) const
        {
            using namespace std::string_literals;

            auto serviceIt = m_RegisteredServices.find(std::type_index(typeid(T)));

            if (serviceIt == m_RegisteredServices.end())
                return std::vector<ServicePtr<T>>();

            auto& services = serviceIt->second;

            std::vector<ServicePtr<T>> result;
            result.reserve(services.size());

            std::transform(
                services.begin(),
                services.end(),
                std::back_inserter(result),
                [this, &container](auto& diService)
                {
                    return GetServiceInstance<T>(diService, container);
                }
            );

            return result;
        }
    private:
        RegisteredServicesMap m_RegisteredServices;

        template <class TService, class TDIService>
        void RegisterServiceInternal(Factory<TService> factory)
        {
            m_RegisteredServices[std::type_index(typeid(TService))].push_back(std::make_shared<TDIService>(factory));
        }

        template <class TService, class TDIService>
        void RegisterServiceInternal(ServicePtr<TService> instance)
        {
            m_RegisteredServices[std::type_index(typeid(TService))].push_back(std::make_shared<TDIService>(instance));
        }

        template <class T>
        ServicePtr<T> GetServiceInstance(const DIServicePtr& diServicePtr, const DIContainer& container) const
        {
            return static_cast<IDIServiceTyped<T>*>(diServicePtr.get())->GetService(container);
        }

        template <class T, bool nothrow>
        ServicePtr<T> GetServiceInternal(const DIContainer& container) const
        {
            using namespace std::string_literals;

            auto serviceIt = m_RegisteredServices.find(std::type_index(typeid(T)));

            if (serviceIt == m_RegisteredServices.end() || serviceIt->second.empty())
                if constexpr (nothrow)
                    return nullptr;
                else
                    throw exceptions::ServiceNotRegisteredException(typeid(T));

            auto& services = serviceIt->second;

            return GetServiceInstance<T>(*services.rbegin(), container);
        }
    };
}
