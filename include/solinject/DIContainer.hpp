#pragma once

#include <map>
#include <memory>
#include <typeinfo>
#include <typeindex>

#include "IDIService.hpp"
#include "IDIServiceTyped.hpp"
#include "DISingletonService.hpp"
#include "DITransientService.hpp"

namespace sol::di
{
    class services::IDIService;

    class DIContainer
    {
    public:
        template <class T>
        using Factory = typename services::IDIServiceTyped<T>::Factory;

        template <class T>
        using ServicePtr = typename services::IDIServiceTyped<T>::ServicePtr;

        DIContainer() {}

        DIContainer(DIContainer& other) = delete;

        DIContainer(DIContainer&& other) noexcept
        {
            m_Services = std::move(other.m_Services);
        }

        DIContainer& operator=(DIContainer& other) = delete;

        DIContainer& operator=(DIContainer&& other) noexcept
        {
            m_Services = std::move(other.m_Services);
            return *this;
        }

        template<class T>
        void RegisterSingletonService(Factory<T> factory)
        {
            RegisterServiceInternal<T, services::DISingletonService<T>>(factory);
        }

        template<class T>
        void RegisterTransientService(Factory<T> factory)
        {
            RegisterServiceInternal<T, services::DITransientService<T>>(factory);
        }

        template<class T>
        ServicePtr<T> GetRequiredServicePtr()
        {
            return GetServicePtrInternal<T>(false);
        }

        template <class T>
        ServicePtr<T> GetServicePtr()
        {
            return GetServicePtrInternal<T>(true);
        }

    private:
        using DIServicePtr = std::unique_ptr<services::IDIService>;

        std::map<std::type_index, DIServicePtr> m_Services;

        template <class TService, class TDIService>
        void RegisterServiceInternal(Factory<TService> factory)
        {
            m_Services[std::type_index(typeid(TService))] = std::make_unique<TDIService>(factory);
        }

        template <class T>
        ServicePtr<T> GetServicePtrInternal(bool nothrow)
        {
            using namespace std::string_literals;

            auto serviceIt = m_Services.find(std::type_index(typeid(T)));

            if (serviceIt == m_Services.end())
                if (nothrow)
                    return nullptr;
                else
                    throw std::out_of_range("Service not found. Service type: "s + typeid(T).name());

            auto diService = serviceIt->second.get();
            auto diServiceTyped = static_cast<services::IDIServiceTyped<T>*>(diService);

            return diServiceTyped->GetService(*this);
        }
    }; // class DIContainer
} // sol::di
