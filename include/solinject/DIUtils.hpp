#pragma once
#include <mutex>
#include <type_traits>

namespace sol::di::utils
{
    class Empty
    {
    public:
        Empty() {}

        template<class...TArgs>
        Empty(const TArgs&...args) {}
    };

    template <class TMutex, bool isEnabled>
    using DiscardableMutex = std::conditional_t<isEnabled, TMutex, Empty>;

    template <class TMutex, bool isEnabled>
    using DiscardableLock = std::conditional_t<isEnabled, std::lock_guard<TMutex>, Empty>;
}
