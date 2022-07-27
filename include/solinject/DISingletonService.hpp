#include <memory>
#include <functional>
#include "DIContainer.hpp"

namespace sol::di { class DIContainer; }

namespace sol::di::services
{
    template<class T>
    class DISingletonService : public IDIServiceTyped<T>
    {
    public:
        using ServicePtr = IDIServiceTyped<T>::ServicePtr;
        using Factory = IDIServiceTyped<T>::Factory;

        DISingletonService(ServicePtr service) : m_ServicePtr(service)
        {
        }

        DISingletonService(Factory factory) : m_Factory(factory), m_ServicePtr(nullptr)
        {
        }

        ServicePtr GetService(sol::di::DIContainer& container) override
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
