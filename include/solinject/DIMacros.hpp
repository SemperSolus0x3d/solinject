#pragma once

#define FROM_DI(class_) (c.GetRequiredServicePtr<class_>())

#define RegisterSingletonService(container, class_, ...) \
    (container).RegisterSingletonService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterSingletonInterface(container, interface_, implementation, ...) \
    (container).RegisterSingletonService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })

#define RegisterTransientService(container, class_, ...) \
    (container).RegisterTransientService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterTransientInterface(container, interface_, implementation, ...) \
    (container).RegisterTransientService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })

#define RegisterSharedService(container, class_, ...) \
    (container).RegisterSharedService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterSharedInterface(container, interface_, implementation, ...) \
    (container).RegisterSharedService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })
