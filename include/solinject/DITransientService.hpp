#pragma once
#include "DIServiceBase.hpp"

namespace sol::di::services
{
    template<class T, bool isThreadsafe>
    class DITransientService : public DIServiceBase<T, isThreadsafe>
    {
    public:
        using Base = DIServiceBase<T, isThreadsafe>;
        using Container = typename Base::Container;
        using ServicePtr = typename Base::ServicePtr;
        using Factory = typename Base::Factory;

        DITransientService(const Factory factory) : m_Factory(factory)
        {
        }

        ServicePtr GetService(const Container& container) override
        {
            return m_Factory(container);
        }

    private:
        Factory m_Factory;
    }; // class DITransientService
} // sol::di::services
