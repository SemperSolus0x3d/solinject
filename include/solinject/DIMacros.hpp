#pragma once

#define FROM_DI(class_) c.GetRequiredServicePtr<class_>()

#define RegisterSingletonService(container, class_, ...) \
    (container).RegisterSingletonService<class_>( \
        [](sol::di::DIContainer& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterSingletonInterface(container, interface_, implementation, ...) \
    (container).RegisterSingletonService<interface_>( \
        [](sol::di::DIContainer& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })

#define RegisterTransientService(container, class_, ...) \
    (container).RegisterTransientService<class_>( \
        [](sol::di::DIContainer& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterTransientInterface(container, interface_, implementation, ...) \
    (container).RegisterTransientService<interface_>( \
        [](sol::di::DIContainer& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })
