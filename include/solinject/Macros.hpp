// SPDX-License-Identifier: LGPL-3.0-or-later

/*
 * solinject - C++ Dependency Injection header-only library
 * Copyright (C) 2022  SemperSolus0x3d
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/// @file

#pragma once

/**
 * @brief Injects a required service from a DI container
 * @param class_ service type
 *
 * The service will be injected as a @ref std::shared_ptr.
 * If the service is not registered, an exception
 * will be thrown.
 *
 * This macro is intended for use as a service constructor argument
 * for the following macros:
 * - @ref RegisterSingletonService()
 * - @ref RegisterSingletonInterface()
 * - @ref RegisterTransientService()
 * - @ref RegisterTransientInterface()
 * - @ref RegisterSharedService()
 * - @ref RegisterSharedInterface()
 * - @ref RegisterScopedService()
 * - @ref RegisterScopedInterface()
 *
 * @see sol::di::exc::ServiceNotRegisteredException
 */
#define FROM_DI(class_) (c.template GetRequiredService<class_>())

/**
 * @brief Injects an optional service from a DI container
 * @param class_ service type
 *
 * The service will be injected as a @ref std::shared_ptr.
 * If the service is not registered, an empty @ref std::shared_ptr
 * will be injected.
 *
 * @warning *Optional* here means that the service **may** or **may not**
 * be registered, and it **doesn't** mean that the service may be
 * registered with `nullptr` or a factory function that returns `nullptr`.
 *
 * This macro is intended for use as a service constructor argument
 * for the following macros:
 * - @ref RegisterSingletonService()
 * - @ref RegisterSingletonInterface()
 * - @ref RegisterTransientService()
 * - @ref RegisterTransientInterface()
 * - @ref RegisterSharedService()
 * - @ref RegisterSharedInterface()
 * - @ref RegisterScopedService()
 * - @ref RegisterScopedInterface()
 */
#define FROM_DI_OPTIONAL(class_) (c.template GetService<class_>())

/**
 * @brief Injects multiple instances of a service from a DI container
 * @param class_ service type
 *
 * The service will be injected as a @ref std::vector< @ref std::shared_ptr<> >.
 * If the service is not registered, an empty
 * @ref std::vector< @ref std::shared_ptr<> > will be injected.
 *
 * This macro is intended for use as a service constructor argument
 * for the following macros:
 * - @ref RegisterSingletonService()
 * - @ref RegisterSingletonInterface()
 * - @ref RegisterTransientService()
 * - @ref RegisterTransientInterface()
 * - @ref RegisterSharedService()
 * - @ref RegisterSharedInterface()
 * - @ref RegisterScopedService()
 * - @ref RegisterScopedInterface()
 */
#define FROM_DI_MULTIPLE(class_) (c.template GetServices<class_>())

/**
 * @brief Service factory
 * @param class_ the service type
 * @param ... the service constructor parameters
 */
#define FACTORY(class_, ...) \
    [](const sol::di::Container& c) \
    { \
        return std::make_shared<class_>(__VA_ARGS__); \
    }

/**
 * @brief Registers a service with singleton lifetime
 *
 * @param container DI container
 * @param class_ service type
 * @param ... service constructor arguments
 *
 * A singleton service is created once and used everywhere.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterSingletonService(container, class_, ...) \
    (container).template RegisterSingletonService<class_>(FACTORY(class_, ## __VA_ARGS__))

/**
 * @brief Registers a service with singleton lifetime
 * as an implementation of a specific interface
 *
 * @param container DI container
 * @param interface_ interface type
 * @param implementation type of the implementation of the interface
 * @param ... service constructor arguments
 *
 * A singleton service is created once and used everywhere.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterSingletonInterface(container, interface_, implementation, ...) \
    (container).template RegisterSingletonService<interface_>(FACTORY(implementation, ## __VA_ARGS__))

/**
 * @brief Registers a service with transient lifetime
 *
 * @param container DI container
 * @param class_ service type
 * @param ... service constructor arguments
 *
 * A transient service is created each time it is requested.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterTransientService(container, class_, ...) \
    (container).template RegisterTransientService<class_>(FACTORY(class_, ## __VA_ARGS__))

/**
 * @brief Registers a service with transient lifetime
 * as an implementation of a specific interface
 *
 * @param container DI container
 * @param interface_ interface type
 * @param implementation type of the implementation of the interface
 * @param ... service constructor arguments
 *
 * A transient service is created each time it is requested.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterTransientInterface(container, interface_, implementation, ...) \
    (container).template RegisterTransientService<interface_>(FACTORY(implementation, ## __VA_ARGS__))

/**
 * @brief Registers a service with shared lifetime
 *
 * @param container DI container
 * @param class_ service type
 * @param ... service constructor arguments
 *
 * A shared service exists while it is used.
 * When the shared service is requested, if an instance
 * already exists, that instance is returned;
 * otherwise a new instance is created.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterSharedService(container, class_, ...) \
    (container).template RegisterSharedService<class_>(FACTORY(class_, ## __VA_ARGS__))

/**
 * @brief Registers a service with shared lifetime
 * as an implementation of a specific interface
 *
 * @param container DI container
 * @param interface_ interface type
 * @param implementation type of the implementation of the interface
 * @param ... service constructor arguments
 *
 * A shared service exists while it is used.
 * When the shared service is requested, if an instance
 * already exists, that instance is returned;
 * otherwise a new instance is created.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterSharedInterface(container, interface_, implementation, ...) \
    (container).template RegisterSharedService<interface_>(FACTORY(implementation, ## __VA_ARGS__))

/**
 * @brief Registers a service with scoped lifetime
 *
 * @param container DI container
 * @param class_ service type
 * @param ... service constructor arguments
 *
 * A scoped service behaves like a singleton inside its scope and derived scopes.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterScopedService(container, class_, ...) \
    (container).template RegisterScopedService<class_>(FACTORY(class_, ## __VA_ARGS__))

/**
 * @brief Registers a service with scoped lifetime
 * as an implementation of a specific interface
 *
 * @param container DI container
 * @param interface_ interface type
 * @param implementation type of the implementation of the interface
 * @param ... service constructor arguments
 *
 * A scoped service behaves like a singleton inside its scope and derived scopes.
 *
 * @see FROM_DI
 * @see FROM_DI_OPTIONAL
 * @see FROM_DI_MULTIPLE
 */
#define RegisterScopedInterface(container, interface_, implementation, ...) \
    (container).template RegisterScopedService<interface_>(FACTORY(implementation, ## __VA_ARGS__))
