#pragma once
#include <memory>
#include <functional>
#include "DIContainer.hpp"

namespace sol::di { class DIContainer; }

namespace sol::di::services
{
    template <class T>
    class IDIServiceTyped abstract : public IDIService
    {
    public:
        using ServicePtr = std::shared_ptr<T>;
        using Factory = std::function<ServicePtr(sol::di::DIContainer&)>;

        virtual ~IDIServiceTyped() {}

        virtual ServicePtr GetService(sol::di::DIContainer& container) = 0;
    };
}
