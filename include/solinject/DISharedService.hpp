#pragma once
#include "DIServiceBase.hpp"

namespace sol::di::services
{
    template<class T, bool isThreadsafe>
    class DISharedService : public DIServiceBase<T, isThreadsafe>
    {
    public:
        using Base = DIServiceBase<T, isThreadsafe>;
        using Container = typename Base::Container;
        using ServicePtr = typename Base::ServicePtr;
        using Factory = typename Base::Factory;
        using ServiceWeakPtr = std::weak_ptr<T>;

        DISharedService(ServicePtr service) : m_ServicePtr(service)
        {
        }

        DISharedService(const Factory factory) : m_Factory(factory), m_ServicePtr()
        {
        }

        ServicePtr GetService(const Container& container) override
        {
            ServicePtr instancePtr = m_ServicePtr.lock();

            if (instancePtr != nullptr)
            {
                return instancePtr;
            }
            else
            {
                ServicePtr newInstancePtr = m_Factory(container);
                m_ServicePtr = ServiceWeakPtr(newInstancePtr);
                return newInstancePtr;
            }
        }

    private:
        ServiceWeakPtr m_ServicePtr;
        Factory m_Factory;
    }; // class DISharedService
} // sol::di::services
