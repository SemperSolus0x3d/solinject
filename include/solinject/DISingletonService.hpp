#pragma once
#include "DIServiceBase.hpp"

namespace sol::di::services
{
    template<class T, bool isThreadsafe>
    class DISingletonService : public DIServiceBase<T, isThreadsafe>
    {
    public:
        using Base = DIServiceBase<T, isThreadsafe>;
        using Container = typename Base::Container;
        using ServicePtr = typename Base::ServicePtr;
        using Factory = typename Base::Factory;

        DISingletonService(ServicePtr service) : m_ServicePtr(service)
        {
        }

        DISingletonService(const Factory factory) : m_Factory(factory), m_ServicePtr(nullptr)
        {
        }

        ServicePtr GetService(const Container& container) override
        {
            if (m_ServicePtr == nullptr)
                m_ServicePtr = m_Factory(container);

            return m_ServicePtr;
        }

    private:
        ServicePtr m_ServicePtr;
        Factory m_Factory;
    }; // class DISingletonService
} // sol::di::services
