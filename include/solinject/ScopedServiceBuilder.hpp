﻿// SPDX-License-Identifier: LGPL-3.0-or-later

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
#include "IScopedServiceBuilder.hpp"

namespace sol::di::impl
{
    /**
     * @brief Builder for scoped DI services
     * @tparam T service type
     */
    template<class TService, class...TServiceParents>
    class ScopedServiceBuilder : public IScopedServiceBuilder
    {
        static_assert(
            std::conjunction_v<std::is_base_of<TServiceParents, TService>...>,
            "The TServiceParents types must be derived from the TService type"
        );
    public:
        /// Base for the @ref ScopedServiceBuilder class
        using Base = IScopedServiceBuilder;

        /// Type of the DI service that is being built
        using DIService = ScopedService<TService, TServiceParents...>;
        
        /// @copydoc IScopedServiceBuilder::DIServicePtr
        using AbstractDIServicePtr = typename Base::DIServicePtr;

        /// @copydoc ScopedService<TService>::Factory
        using Factory = typename DIService::Factory;

        /**
         * @brief Constructor
         * @param factory factory function
         */
        ScopedServiceBuilder(Factory factory) : m_Factory(factory)
        {
        }

        AbstractDIServicePtr BuildDIService() const override
        {
            return std::make_shared<DIService>(m_Factory);
        }
    private:
        /// Factory function
        Factory m_Factory;
    };
}
