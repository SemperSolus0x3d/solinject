#pragma once
#include <memory>
#include <functional>

namespace sol::di { template <bool isThreadsafe> class DIContainer; }

namespace sol::di::services
{
    template <class T, bool isThreadsafe>
    class IDIServiceTyped abstract : public IDIService
    {
    public:
        using Container = sol::di::DIContainer<isThreadsafe>;
        using ServicePtr = std::shared_ptr<T>;
        using Factory = std::function<ServicePtr(const Container&)>;

        virtual ~IDIServiceTyped() {}

        virtual ServicePtr GetService(const Container& container) = 0;
    };
}
