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
#include <mutex>

#include "services/IDIService.hpp"
#include "services/IDIServiceTyped.hpp"
#include "services/DISingletonService.hpp"
#include "services/DITransientService.hpp"
#include "services/DISharedService.hpp"
#include "exceptions/ServiceNotRegisteredException.hpp"
#include "DIUtils.hpp"

namespace sol::di
{
    namespace services
    {
        class IDIService;
    }

    template <bool isThreadsafe = true>
    class DIContainer
    {
    public:
        template <class T>
        using Factory = typename services::IDIServiceTyped<T, isThreadsafe>::Factory;

        template <class T>
        using ServicePtr = typename services::IDIServiceTyped<T, isThreadsafe>::ServicePtr;

        DIContainer() {}

        DIContainer(const DIContainer& other) = delete;

        DIContainer(DIContainer&& other) noexcept : DIContainer()
        {
            swap(*this, other);
        }

        DIContainer& operator=(const DIContainer& other) = delete;

        DIContainer& operator=(DIContainer&& other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        friend void swap(DIContainer<isThreadsafe>& a, DIContainer<isThreadsafe>& b) noexcept
        {
            using namespace utils;
            using std::swap;

            if (&a == &b)
                return;

            DiscardableScopedLock<isThreadsafe, Mutex, Mutex> lock(a.m_Mutex, b.m_Mutex);

            swap(a.m_Services, b.m_Services);
        }

        template<class T>
        void RegisterSingletonService(const Factory<T> factory)
        {
            RegisterServiceInternal<T, services::DISingletonService<T, isThreadsafe>>(factory);
        }

        template<class T>
        void RegisterSingletonService(ServicePtr<T> instance)
        {
            RegisterServiceInternal<T, services::DISingletonService<T, isThreadsafe>>(instance);
        }

        template<class T>
        void RegisterTransientService(const Factory<T> factory)
        {
            RegisterServiceInternal<T, services::DITransientService<T, isThreadsafe>>(factory);
        }

        template<class T>
        void RegisterSharedService(const Factory<T> factory)
        {
            RegisterServiceInternal<T, services::DISharedService<T, isThreadsafe>>(factory);
        }

        template<class T>
        ServicePtr<T> GetRequiredService() const
        {
            return GetServiceInternal<T, false>();
        }

        template <class T>
        ServicePtr<T> GetService() const
        {
            return GetServiceInternal<T, true>();
        }

        template <class T>
        std::vector<ServicePtr<T>> GetServices() const
        {
            using namespace std::string_literals;

            auto lock = LockMutex();

            auto serviceIt = m_Services.find(std::type_index(typeid(T)));

            if (serviceIt == m_Services.end())
                return std::vector<ServicePtr<T>>();

            auto& services = serviceIt->second;

            std::vector<ServicePtr<T>> result;
            result.reserve(services.size());

            std::transform(
                services.begin(),
                services.end(),
                std::back_inserter(result),
                [this](auto& diService)
                {
                    return GetServiceInstance<T>(diService);
                }
            );

            return result;
        }

    private:
        using DIServicePtr = std::unique_ptr<services::IDIService>;
        using Mutex = std::recursive_mutex;

        std::map<std::type_index, std::vector<DIServicePtr>> m_Services;

        mutable utils::DiscardableMutex<Mutex, isThreadsafe> m_Mutex;

        template <class T>
        ServicePtr<T> GetServiceInstance(const DIServicePtr& diServicePtr) const
        {
            return static_cast<services::IDIServiceTyped<T, isThreadsafe>*>(diServicePtr.get())->GetService(*this);
        }

        template <class TService, class TDIService>
        void RegisterServiceInternal(Factory<TService> factory)
        {
            auto lock = LockMutex();

            m_Services[std::type_index(typeid(TService))].push_back(std::make_unique<TDIService>(factory));
        }

        template <class TService, class TDIService>
        void RegisterServiceInternal(ServicePtr<TService> instance)
        {
            auto lock = LockMutex();

            m_Services[std::type_index(typeid(TService))].push_back(std::make_unique<TDIService>(instance));
        }

        template <class T, bool nothrow>
        ServicePtr<T> GetServiceInternal() const
        {
            using namespace std::string_literals;

            auto lock = LockMutex();

            auto serviceIt = m_Services.find(std::type_index(typeid(T)));

            if (serviceIt == m_Services.end() || serviceIt->second.empty())
                if constexpr (nothrow)
                    return nullptr;
                else
                    throw exceptions::ServiceNotRegisteredException(typeid(T));

            auto& services = serviceIt->second;

            return GetServiceInstance<T>(*services.rbegin());
        }

        utils::DiscardableLock<Mutex, isThreadsafe> LockMutex() const
        {
            return utils::DiscardableLock<Mutex, isThreadsafe>(m_Mutex);
        }
    }; // class DIContainer
} // sol::di
