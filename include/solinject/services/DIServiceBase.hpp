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

#pragma once
#include <mutex>
#include "IDIServiceTyped.hpp"
#include "solinject/DIUtils.hpp"
#include "solinject/exceptions/CircularDependencyException.hpp"

namespace sol::di::services
{
    template <class T, bool isThreadsafe>
    class DIServiceBase : public IDIServiceTyped<T, isThreadsafe>
    {
    public:
        using Base = IDIServiceTyped<T, isThreadsafe>;
        using ServicePtr = typename Base::ServicePtr;
        using Factory = typename Base::Factory;
        using Container = typename Base::Container;

        virtual ~DIServiceBase() = 0;

        virtual ServicePtr GetService(const Container& container)
        {
            if (m_IsLocked)
            {
                m_IsLocked = false;
                throw exceptions::CircularDependencyException(typeid(T));
            }

            m_IsLocked = true;
            auto service = GetServiceInternal(container);
            m_IsLocked = false;

            return service;
        }

    protected:
        bool m_IsLocked = false;

        virtual ServicePtr GetServiceInternal(const Container& container) = 0;
    };

    template <class T, bool isThreadsafe>
    DIServiceBase<T, isThreadsafe>::~DIServiceBase() {}
}
