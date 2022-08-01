#pragma once
#include <memory>
#include <functional>

namespace sol::di { template <bool isThreadsafe> class DIContainer; }

namespace sol::di::services
{
    template <class T, bool isThreadsafe>
    class IDIServiceTyped : public IDIService
    {
    public:
        using Container = typename sol::di::DIContainer<isThreadsafe>;
        using ServicePtr = typename std::shared_ptr<T>;
        using Factory = typename std::function<ServicePtr(const Container&)>;

        virtual ~IDIServiceTyped() = 0;

        virtual ServicePtr GetService(const Container& container) = 0;
    };

    template <class T, bool isThreadsafe>
    IDIServiceTyped<T, isThreadsafe>::~IDIServiceTyped() {}
}
