#pragma once

#define FROM_DI(class_) (c.template GetRequiredServicePtr<class_>())

#define RegisterSingletonService(container, class_, ...) \
    (container).template RegisterSingletonService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterSingletonInterface(container, interface_, implementation, ...) \
    (container).template RegisterSingletonService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })

#define RegisterTransientService(container, class_, ...) \
    (container).template RegisterTransientService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterTransientInterface(container, interface_, implementation, ...) \
    (container).template RegisterTransientService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })

#define RegisterSharedService(container, class_, ...) \
    (container).template RegisterSharedService<class_>( \
        [](const auto& c) \
        { \
            return std::make_unique<class_>(__VA_ARGS__); \
        })

#define RegisterSharedInterface(container, interface_, implementation, ...) \
    (container).template RegisterSharedService<interface_>( \
        [](const auto& c) \
        { \
            return std::make_unique<implementation>(__VA_ARGS__); \
        })
