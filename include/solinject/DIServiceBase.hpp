#pragma once
#include <mutex>
#include "IDIServiceTyped.hpp"
#include "DIUtils.hpp"

namespace sol::di::services
{
    template <class T, bool isThreadsafe>
    class DIServiceBase abstract : public IDIServiceTyped<T, isThreadsafe>
    {
    public:
        using Base = IDIServiceTyped<T, isThreadsafe>;
        using ServicePtr = Base::ServicePtr;
        using Factory = Base::Factory;

        virtual ~DIServiceBase() {}
    };
}
