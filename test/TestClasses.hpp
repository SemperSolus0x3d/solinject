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
#include <memory>

namespace sol::di::test
{
    class TestA {};

    class TestB
    {
    public:
        TestB(std::shared_ptr<TestA> a) {}
    };

    class TestC
    {
    public:
        TestC(std::shared_ptr<TestA> a, std::shared_ptr<TestB> b) {}
    };

    class ITestD
    {
    public:
        virtual ~ITestD() {}
    };

    class TestD : public ITestD
    {
    public:
        TestD(std::shared_ptr<TestC> c) {}
    };

    class TestE
    {
    public:
        TestE(std::shared_ptr<ITestD> d) {}
    };

    class TestF
    {
    public:
        TestF(std::shared_ptr<TestE> e) {}
    };

    class TestG
    {
    public:
        TestG(std::vector<std::shared_ptr<TestF>> fs) {}
    };

    class SameInstanceTestClass
    {
    public:
        SameInstanceTestClass() { m_Id = NextId++; }
        SameInstanceTestClass(int id) : m_Id(id) { }
        int Id() { return m_Id; }

        static void ResetIds() { NextId = 0; }
    private:
        static int NextId;
        int m_Id;
    };

    int SameInstanceTestClass::NextId = 0;

    class CircularDependencyTestClassB;

    class CircularDependencyTestClassA
    {
    public:
        CircularDependencyTestClassA(std::shared_ptr<CircularDependencyTestClassB> b) {}
    };

    class CircularDependencyTestClassB
    {
    public:
        CircularDependencyTestClassB(std::shared_ptr<CircularDependencyTestClassA> a) {}
    };

    class CircularDependencyTestClassC
    {
    public:
        CircularDependencyTestClassC(std::shared_ptr<CircularDependencyTestClassC> c) {}
    };
} // sol::di::test
