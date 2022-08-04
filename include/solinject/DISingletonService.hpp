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

#pragma once
#include "DIServiceBase.hpp"

namespace sol::di::services
{
    template<class T, bool isThreadsafe>
    class DISingletonService : public DIServiceBase<T, isThreadsafe>
    {
    public:
        using Base = DIServiceBase<T, isThreadsafe>;
        using Container = typename Base::Container;
        using ServicePtr = typename Base::ServicePtr;
        using Factory = typename Base::Factory;

        DISingletonService(ServicePtr service) : m_ServicePtr(service)
        {
        }

        DISingletonService(const Factory factory) : m_Factory(factory), m_ServicePtr(nullptr)
        {
        }

        ServicePtr GetService(const Container& container) override
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
