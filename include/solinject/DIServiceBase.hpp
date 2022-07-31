#pragma once
#include <mutex>
#include "IDIServiceTyped.hpp"
#include "DIUtils.hpp"

namespace sol::di::services
{
    template <class T, bool isThreadsafe>
    class DIServiceBase : public IDIServiceTyped<T, isThreadsafe>
    {
    public:
        using Base = IDIServiceTyped<T, isThreadsafe>;
        using ServicePtr = typename Base::ServicePtr;
        using Factory = typename Base::Factory;

        virtual ~DIServiceBase() = 0;
    };

    template <class T, bool isThreadsafe>
    DIServiceBase<T, isThreadsafe>::~DIServiceBase() {}
}
