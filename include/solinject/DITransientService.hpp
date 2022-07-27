#include <memory>
#include <functional>
#include "DIContainer.hpp"

namespace sol::di { class DIContainer; }

namespace sol::di::services
{
    template<class T>
    class DITransientService : public IDIServiceTyped<T>
    {
    public:
        using ServicePtr = IDIServiceTyped<T>::ServicePtr;
        using Factory = IDIServiceTyped<T>::Factory;

        DITransientService(Factory factory) : m_Factory(factory)
        {
        }

        ServicePtr GetService(sol::di::DIContainer& container) override
        {
            return m_Factory(container);
        }

    private:
        Factory m_Factory;
    }; // class DITransientService
} // sol::di::services
