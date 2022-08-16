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

#include "Defines.hpp"
#include "services/IDIService.hpp"
#include "services/IDIServiceTyped.hpp"
#include "services/DIRegisteredServices.hpp"
#include "services/DIScopedServiceBuilders.hpp"
#include "Utils.hpp"

namespace sol::di
{
    namespace services
    {
        class IDIService;
    }

    class DIContainerScope;

    class DIContainer
    {
    public:
        template <class T>
        using Factory = typename services::IDIServiceTyped<T>::Factory;

        template <class T>
        using ServicePtr = typename services::IDIServiceTyped<T>::ServicePtr;

        DIContainer() : m_Mutex(std::make_shared<Mutex>()) {}

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

        friend void swap(DIContainer& a, DIContainer& b) noexcept
        {
            using std::swap;

            if (&a == &b)
                return;

            ScopedLock lock(*a.m_Mutex, *b.m_Mutex);

            swap(a.m_RegisteredServices, b.m_RegisteredServices);
            swap(a.m_ScopedServiceBuilders, b.m_ScopedServiceBuilders);
            swap(a.m_Mutex, b.m_Mutex);
        }

        DIContainerScope CreateScope() const;

        template<class T>
        void RegisterSingletonService(const Factory<T> factory)
        {
            auto lock = LockMutex();
            m_RegisteredServices.template RegisterSingletonService<T>(factory);
        }

        template<class T>
        void RegisterSingletonService(ServicePtr<T> instance)
        {
            solinject_assert(instance != nullptr);

            if (instance == nullptr)
                throw std::invalid_argument("instance was nullptr");

            auto lock = LockMutex();
            m_RegisteredServices.template RegisterSingletonService<T>(instance);
        }

        template<class T>
        void RegisterTransientService(const Factory<T> factory)
        {
            auto lock = LockMutex();
            m_RegisteredServices.template RegisterTransientService<T>(factory);
        }

        template<class T>
        void RegisterSharedService(const Factory<T> factory)
        {
            auto lock = LockMutex();
            m_RegisteredServices.template RegisterSharedService<T>(factory);
        }

        template<class T>
        void RegisterScopedService(const Factory<T> factory)
        {
            auto lock = LockMutex();
            m_ScopedServiceBuilders.template RegisterScopedService<T>(factory);
        }

        template<class T>
        ServicePtr<T> GetRequiredService() const
        {
            auto lock = LockMutex();
            return m_RegisteredServices.template GetRequiredService<T>(*this);
        }

        template <class T>
        ServicePtr<T> GetService() const
        {
            auto lock = LockMutex();
            return m_RegisteredServices.template GetService<T>(*this);
        }

        template <class T>
        std::vector<ServicePtr<T>> GetServices() const
        {
            auto lock = LockMutex();
            return m_RegisteredServices.template GetServices<T>(*this);
        }

    protected:
        #ifndef SOLINJECT_NOTHREADSAFE
            using Mutex = std::recursive_mutex;
            using Lock = std::lock_guard<Mutex>;
            using ScopedLock = std::scoped_lock<Mutex, Mutex>;
        #else
            using Mutex = utils::Empty;
            using Lock = utils::Empty;
            using ScopedLock = utils::Empty;
        #endif

        using MutexPtr = std::shared_ptr<Mutex>;

        DIContainer(
            services::DIRegisteredServices&& services,
            MutexPtr mutexPtr
        ) :
            m_RegisteredServices(std::move(services)),
            m_Mutex(mutexPtr)
        {
        }

    private:
        services::DIRegisteredServices m_RegisteredServices;
        services::DIScopedServiceBuilders m_ScopedServiceBuilders;

        MutexPtr m_Mutex;

        Lock LockMutex() const
        {
            return Lock(*m_Mutex);
        }
    }; // class DIContainer

    class DIContainerScope : public DIContainer
    {
    public:
        using MutexPtr = DIContainer::MutexPtr;

        DIContainerScope(
            services::DIRegisteredServices&& services,
            MutexPtr mutexPtr
        ) : DIContainer(std::move(services), mutexPtr)
        {
        }

        DIContainerScope(const DIContainerScope&) = delete;
        DIContainerScope(DIContainerScope&& other) : DIContainer(std::move(other))
        {
        }

        DIContainerScope& operator=(const DIContainerScope&) = delete;
        DIContainerScope& operator=(DIContainerScope&& other)
        {
            DIContainer::operator=(std::move(other));
            return *this;
        }
    }; // class DIContainerScope

    DIContainerScope DIContainer::CreateScope() const
    {
        using namespace services;

        auto lock = LockMutex();

        DIRegisteredServices diServices = m_RegisteredServices;
        diServices.Merge(m_ScopedServiceBuilders.BuildDIServices());

        return DIContainerScope(std::move(diServices), m_Mutex);
    }
} // sol::di
