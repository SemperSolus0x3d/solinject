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

#include <map>
#include <vector>
#include <algorithm>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <mutex>

#include "Defines.hpp"
#include "IService.hpp"
#include "IServiceTyped.hpp"
#include "RegisteredServices.hpp"
#include "ScopedServiceBuilders.hpp"
#include "Utils.hpp"

namespace sol::di
{
    namespace impl { class IService; }

    /**
     * @brief Dependency Injection container
     * @headerfile Container.hpp solinject.hpp
     */
    class Container
    {
    public:
        /**
         * @copydoc impl::IServiceTyped<T>::Factory
         * @tparam T the service type
         */
        template <class T>
        using Factory = typename impl::IServiceTyped<T>::Factory;

        /**
         * @copydoc impl::IServiceTyped<T>::ServicePtr
         * @tparam T service type
         */
        template <class T>
        using ServicePtr = typename impl::IServiceTyped<T>::ServicePtr;

        /// @copydoc impl::RegisteredServices::DIServicePtr
        using DIServicePtr = impl::RegisteredServices::DIServicePtr;

        /// @copydoc impl::ScopedServiceBuilders::ScopedServiceBuilderPtr
        using ScopedServiceBuilderPtr = impl::ScopedServiceBuilders::ScopedServiceBuilderPtr;

        /// Default constructor
        Container() : m_Mutex(std::make_shared<Mutex>()) {}

        /// Copy constructor (deleted)
        Container(const Container& other) = delete;

        /// Move constructor
        Container(Container&& other) noexcept : Container()
        {
            swap(*this, other);
        }

        /// Copy-assignment operator (deleted)
        Container& operator=(const Container& other) = delete;

        /// Move-assignment operator
        Container& operator=(Container&& other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        /// Swaps two @ref Container instances
        friend void swap(Container& a, Container& b) noexcept
        {
            using std::swap;

            if (&a == &b)
                return;

            ScopedLock lock(*a.m_Mutex, *b.m_Mutex);

            swap(a.m_RegisteredServices, b.m_RegisteredServices);
            swap(a.m_ScopedServiceBuilders, b.m_ScopedServiceBuilders);
            swap(a.m_Mutex, b.m_Mutex);
        }

        /**
         * @brief Creates a scoped container
         * from the current container
         * @returns Scoped @ref Container instance
         */
        Container CreateScope() const
        {
            using namespace impl;

            auto lock = LockMutex();

            RegisteredServices diServices = m_RegisteredServices;
            diServices.Merge(m_ScopedServiceBuilders.BuildDIServices());

            return Container(std::move(diServices), m_Mutex);
        }

        /**
         * @brief Tells if the container
         * is a scope container
         * @returns `true` if the container is a
         * scope container, `false` otherwise
         */
        bool IsScope() { return m_IsScope; }

        /**
         * @brief Registers a service with singleton lifetime
         * @tparam T service type
         * @param factory factory function
         */
        template<class T>
        void RegisterSingletonService(Factory<T> factory)
        {
            auto lock = LockMutex();
            m_RegisteredServices.template RegisterSingletonService<T>(factory);
        }

        /**
         * @brief Registers a service with singleton lifetime
         * @tparam T service type
         * @param instance pointer to an instance of the service
         */
        template<class T>
        void RegisterSingletonService(ServicePtr<T> instance)
        {
            solinject_assert(instance != nullptr);

            if (instance == nullptr)
                throw std::invalid_argument("instance was nullptr");

            auto lock = LockMutex();
            m_RegisteredServices.template RegisterSingletonService<T>(instance);
        }

        /**
         * @brief Registers a service with transient lifetime
         * @tparam T service type
         * @param factory factory function
         */
        template<class T>
        void RegisterTransientService(Factory<T> factory)
        {
            auto lock = LockMutex();
            m_RegisteredServices.template RegisterTransientService<T>(factory);
        }

        /**
         * @brief Registers a service with shared lifetime
         * @tparam T service type
         * @param factory factory function
         */
        template<class T>
        void RegisterSharedService(Factory<T> factory)
        {
            auto lock = LockMutex();
            m_RegisteredServices.template RegisterSharedService<T>(factory);
        }

        /**
         * @brief Registers a service with scoped lifetime
         * @tparam T service type
         * @param factory factory function
         */
        template<class T>
        void RegisterScopedService(Factory<T> factory)
        {
            auto lock = LockMutex();
            m_ScopedServiceBuilders.template RegisterScopedService<T>(factory);
        }

        /**
         * @brief Registers a service
         * @param type service type
         * @param diService pointer to a DI service instance
         * @warning This method is intended for use by the
         * @ref ContainerBuilder class only
         */
        void RegisterService(std::type_index type, DIServicePtr diService)
        {
            m_RegisteredServices.RegisterService(type, diService);
        }

        /**
         * @brief Registers a scoped service builder
         * @param type service type
         * @param serviceBuilder service builder
         * @warning This method is intended for use by the
         * @ref ContainerBuilder class only
         */
        void RegisterScopedServiceBuilder(std::type_index type, ScopedServiceBuilderPtr serviceBuilder)
        {
            m_ScopedServiceBuilders.RegisterScopedService(type, serviceBuilder);
        }

        /**
         * @brief Resolves a required service
         * @tparam T service type
         * @returns Pointer to an instance of the service
         * @throws sol::di::exc::ServiceNotRegisteredException
         */
        template<class T>
        ServicePtr<T> GetRequiredService() const
        {
            auto lock = LockMutex();
            return m_RegisteredServices.template GetRequiredService<T>(*this);
        }

        /**
         * @brief Resolves an optional service
         * @tparam T service type
         * @returns Pointer to an instance of the service or `nullptr`
         * if the service is not registered
         */
        template <class T>
        ServicePtr<T> GetService() const
        {
            auto lock = LockMutex();
            return m_RegisteredServices.template GetService<T>(*this);
        }

        /**
         * @brief Resolves services
         * @tparam T the service type
         * @returns @ref std::vector of pointers to service instances
         * or empty vector if the service is not registered
         */
        template <class T>
        std::vector<ServicePtr<T>> GetServices() const
        {
            auto lock = LockMutex();
            return m_RegisteredServices.template GetServices<T>(*this);
        }

    private:
        #ifndef SOLINJECT_NOTHREADSAFE
            using Mutex = std::recursive_mutex;
            using Lock = std::lock_guard<Mutex>;
            using ScopedLock = std::scoped_lock<Mutex, Mutex>;
        #else
            using Mutex = impl::Empty;
            using Lock = impl::Empty;
            using ScopedLock = impl::Empty;
        #endif

        using MutexPtr = std::shared_ptr<Mutex>;

        /**
         * @brief Scoped container constructor
         * @param services registered services
         * @param mutexPtr pointer to a mutex
         */
        Container(
            impl::RegisteredServices&& services,
            MutexPtr mutexPtr
        ) :
            m_RegisteredServices(std::move(services)),
            m_Mutex(mutexPtr),
            m_IsScope(true)
        {
            solinject_req_assert(mutexPtr != nullptr);
        }

        /// Registered services
        impl::RegisteredServices m_RegisteredServices;

        /// Scoped service builders
        impl::ScopedServiceBuilders m_ScopedServiceBuilders;

        /// Pointer to a mutex
        MutexPtr m_Mutex;

        /**
         * @brief Field, indicating if the container
         * is a scope container.
         */
        bool m_IsScope = false;

        /**
         * @brief Locks the mutex
         * @returns a lock object
         */
        Lock LockMutex() const
        {
            return Lock(*m_Mutex);
        }
    }; // class Container
} // sol::di
