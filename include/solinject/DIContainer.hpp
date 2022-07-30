#pragma once

#include <map>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <mutex>

#include "IDIService.hpp"
#include "IDIServiceTyped.hpp"
#include "DISingletonService.hpp"
#include "DITransientService.hpp"
#include "DISharedService.hpp"
#include "DIUtils.hpp"

namespace sol::di
{
    class services::IDIService;

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

        DIContainer(DIContainer&& other) noexcept
        {
            m_Services = std::move(other.m_Services);
        }

        DIContainer& operator=(const DIContainer& other) = delete;

        DIContainer& operator=(DIContainer&& other) noexcept
        {
            m_Services = std::move(other.m_Services);
            return *this;
        }

        template<class T>
        void RegisterSingletonService(const Factory<T> factory)
        {
            RegisterServiceInternal<T, services::DISingletonService<T, isThreadsafe>>(factory);
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
        ServicePtr<T> GetRequiredServicePtr() const
        {
            return GetServicePtrInternal<T, false>();
        }

        template <class T>
        ServicePtr<T> GetServicePtr() const
        {
            return GetServicePtrInternal<T, true>();
        }

    private:
        using DIServicePtr = std::unique_ptr<services::IDIService>;
        using Mutex = std::recursive_mutex;

        std::map<std::type_index, DIServicePtr> m_Services;

        mutable utils::DiscardableMutex<Mutex, isThreadsafe> m_Mutex;

        template <class TService, class TDIService>
        void RegisterServiceInternal(Factory<TService> factory)
        {
            auto lock = LockMutex();

            m_Services[std::type_index(typeid(TService))] = std::make_unique<TDIService>(factory);
        }

        template <class T, bool nothrow>
        ServicePtr<T> GetServicePtrInternal() const
        {
            using namespace std::string_literals;

            auto lock = LockMutex();

            auto serviceIt = m_Services.find(std::type_index(typeid(T)));

            if (serviceIt == m_Services.end())
                if constexpr (nothrow)
                    return nullptr;
                else
                    throw std::out_of_range("Service not found. Service type: "s + typeid(T).name());

            auto diService = serviceIt->second.get();
            auto diServiceTyped = static_cast<services::IDIServiceTyped<T, isThreadsafe>*>(diService);

            return diServiceTyped->GetService(*this);
        }

        utils::DiscardableLock<Mutex, isThreadsafe> LockMutex() const
        {
            return utils::DiscardableLock<Mutex, isThreadsafe>(m_Mutex);
        }
    }; // class DIContainer
} // sol::di
