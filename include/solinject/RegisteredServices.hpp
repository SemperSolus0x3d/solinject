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

#include "solinject/Defines.hpp"
#include "IService.hpp"
#include "IServiceTyped.hpp"
#include "SingletonService.hpp"
#include "TransientService.hpp"
#include "SharedService.hpp"
#include "ScopedService.hpp"
#include "solinject/exceptions/ServiceNotRegisteredException.hpp"
#include "solinject/Utils.hpp"

namespace sol::di::impl
{
    /// Registered DI services collection
    class RegisteredServices
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

        /// Pointer to a DI service instance
        using DIServicePtr = std::shared_ptr<IService>;

        /// Map of registered DI services
        using RegisteredServicesMap = std::map<std::type_index, std::vector<DIServicePtr>>;

        /// Default constructor
        RegisteredServices() {}

        /**
         * @brief Constructor
         * @param services map of DI services
         */
        RegisteredServices(RegisteredServicesMap services) :
            m_RegisteredServices(std::move(services))
        {
        }

        /// Copy constructor
        RegisteredServices(const RegisteredServices& other) :
            m_RegisteredServices(other.m_RegisteredServices)
        {
        }

        /// Move constructor
        RegisteredServices(RegisteredServices&& other) : RegisteredServices()
        {
            swap(*this, other);
        }

        /// Copy-assignment operator
        RegisteredServices& operator=(RegisteredServices other)
        {
            swap(*this, other);
            return *this;
        }

        /// Move-assignment operator
        RegisteredServices& operator=(RegisteredServices&& other)
        {
            swap(*this, other);
            return *this;
        }

        /// Swaps two @ref RegisteredServices instances
        friend void swap(RegisteredServices& a, RegisteredServices& b)
        {
            using std::swap;

            swap(a.m_RegisteredServices, b.m_RegisteredServices);
        }

        /**
         * @brief Merges other @ref RegisteredServices instance
         * into this instance
         * @param other other @ref RegisteredServices instance
         */
        void Merge(RegisteredServices other)
        {
            for (auto& pair : other.m_RegisteredServices)
            {
                auto& source = pair.second;
                auto& destination = m_RegisteredServices[pair.first];

                impl::ConcatenateVectors(destination, std::move(source));
            }
        }

        /**
         * @brief Registers a service with singleton lifetime
         * @param factory factory function
         * @tparam T service type
         */
        template<class T>
        void RegisterSingletonService(Factory<T> factory)
        {
            RegisterServiceInternal<T, SingletonService<T>>(factory);
        }

        /**
         * @brief Registers a service with singleton lifetime
         * @param instance pointer to an instance of the service
         * @tparam T service type
         */
        template<class T>
        void RegisterSingletonService(ServicePtr<T> instance)
        {
            RegisterServiceInternal<T, SingletonService<T>>(instance);
        }

        /**
         * @brief Registers a service with transient lifetime
         * @param factory factory function
         * @tparam T service type
         */
        template<class T>
        void RegisterTransientService(Factory<T> factory)
        {
            RegisterServiceInternal<T, TransientService<T>>(factory);
        }

        /**
         * @brief Registers a service with shared lifetime
         * @param factory factory function
         * @tparam T service type
         */
        template<class T>
        void RegisterSharedService(Factory<T> factory)
        {
            RegisterServiceInternal<T, SharedService<T>>(factory);
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
            RegisterServiceInternal(type, diService);
        }

        /**
         * @brief Resolves a required service
         * @tparam T service type
         * @param[in] container DI container
         * @returns Pointer to an instance of the service
         * @throws sol::di::exc::ServiceNotRegisteredException
         */
        template<class T>
        ServicePtr<T> GetRequiredService(const Container& container) const
        {
            return GetServiceInternal<T, false>(container);
        }

        /**
         * @brief Resolves an optional service
         * @tparam T service type
         * @param[in] container DI container
         * @returns Pointer to an instance of the service or `nullptr`
         * if the service is not registered
         */
        template <class T>
        ServicePtr<T> GetService(const Container& container) const
        {
            return GetServiceInternal<T, true>(container);
        }

        /**
         * @brief Resolves services
         * @tparam T the service type
         * @param[in] container DI container
         * @returns @ref std::vector of pointers to service instances
         * or empty vector if the service is not registered
         */
        template <class T>
        std::vector<ServicePtr<T>> GetServices(const Container& container) const
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
        /// Registered services
        RegisteredServicesMap m_RegisteredServices;

        /**
         * @brief Registers a service
         * @param type service type
         * @param diService pointer to a DI service instance
         */
        void RegisterServiceInternal(std::type_index type, DIServicePtr diService)
        {
            m_RegisteredServices[type].push_back(diService);
        }

        /**
         * @brief Registers a service
         * @tparam TService service type
         * @tparam TDISevice DI service type
         * @param factory factory function
         */
        template <class TService, class TDIService>
        void RegisterServiceInternal(Factory<TService> factory)
        {
            RegisterServiceInternal(
                std::type_index(typeid(TService)),
                std::make_shared<TDIService>(factory)
            );
        }

        /**
         * @brief Registers a service
         * @tparam TService service type
         * @tparam TDISevice DI service type
         * @param instance pointer to an instance of the service
         */
        template <class TService, class TDIService>
        void RegisterServiceInternal(ServicePtr<TService> instance)
        {
            solinject_req_assert(instance != nullptr);

            m_RegisteredServices[std::type_index(typeid(TService))].push_back(std::make_shared<TDIService>(instance));
        }

        /**
         * @brief Resolves a service from a DI service
         * @tparam T service type
         * @param[in] diServicePtr pointer to an instance of the DI service
         * @param[in] container DI container
         * @returns pointer to an instance of the service
         */
        template <class T>
        ServicePtr<T> GetServiceInstance(const DIServicePtr& diServicePtr, const Container& container) const
        {
            solinject_req_assert(diServicePtr != nullptr);

            auto castedDiServicePtr = std::dynamic_pointer_cast<IServiceTyped<T>>(diServicePtr);

            solinject_req_assert(castedDiServicePtr != nullptr && "The service is resolvable as T");

            return castedDiServicePtr->GetService(container);
        }

        /**
         * @brief Resolves a service
         * @tparam T service type
         * @tparam nothrow value, indicating if the method should throw
         * exception if the service is not registered
         * @param[in] container DI container
         * @returns pointer to an instance of the service
         */
        template <class T, bool nothrow>
        ServicePtr<T> GetServiceInternal(const Container& container) const
        {
            using namespace std::string_literals;

            auto serviceIt = m_RegisteredServices.find(std::type_index(typeid(T)));

            bool serviceFound =
                serviceIt != m_RegisteredServices.end() &&
                !serviceIt->second.empty();

            if constexpr (!nothrow)
                solinject_assert(serviceFound);

            if (!serviceFound)
                if constexpr (nothrow)
                    return nullptr;
                else
                    throw exc::ServiceNotRegisteredException(typeid(T));

            auto& services = serviceIt->second;

            return GetServiceInstance<T>(*services.rbegin(), container);
        }
    };
}
