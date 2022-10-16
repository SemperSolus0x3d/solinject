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

#include <iostream>
#include <vector>
#include <thread>
#include <assert.h>
#include <solinject.hpp>
#include <solinject-macros.hpp>

#include "TestClasses.hpp"

using namespace sol::di;

void RunTests();

int main()
{
    try
    {
        RunTests();

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return -1;
}

void ItRegistersAndReturnsServices()
{
    using namespace sol::di::test;

    Container container;

    RegisterSingletonService(container, TestA);
    RegisterTransientService(container, TestB, FROM_DI(TestA));
    RegisterSharedService(container, TestC, FROM_DI(TestA), FROM_DI(TestB));
    RegisterSingletonInterface(container, ITestD, TestD, FROM_DI(TestC));
    RegisterTransientService(container, TestE, FROM_DI(ITestD));
    RegisterSharedService(container, TestF, FROM_DI(TestE));
    RegisterSingletonService(container, TestG, FROM_DI_MULTIPLE(TestF));

    auto d = container.template GetRequiredService<ITestD>();
    auto a = container.template GetRequiredService<TestA>();
    auto g = container.template GetRequiredService<TestG>();
    auto e = container.template GetRequiredService<TestE>();
    auto c = container.template GetRequiredService<TestC>();
    auto b = container.template GetRequiredService<TestB>();
    auto f = container.template GetRequiredService<TestF>();

    assert(a != nullptr);
    assert(b != nullptr);
    assert(c != nullptr);
    assert(d != nullptr);
    assert(e != nullptr);
    assert(f != nullptr);
    assert(g != nullptr);
}

void ItReturnsSameSingletonInstance()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;

    RegisterSingletonService(container, SameInstanceTestClass);

    auto instance1 = container.template GetRequiredService<SameInstanceTestClass>();
    auto instance2 = container.template GetRequiredService<SameInstanceTestClass>();

    assert(instance1->Id() == instance2->Id());
}

void ItRegistersSingletonByInstance()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;

    container.template RegisterSingletonService<SameInstanceTestClass>(
        std::make_shared<SameInstanceTestClass>(666)
    );

    auto instance = container.template GetRequiredService<SameInstanceTestClass>();

    assert(instance->Id() == 666);
}

void ItReturnsDifferentTransientInstances()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;

    RegisterTransientService(container, SameInstanceTestClass);

    auto instance1 = container.template GetRequiredService<SameInstanceTestClass>();
    auto instance2 = container.template GetRequiredService<SameInstanceTestClass>();

    assert(instance1->Id() != instance2->Id());
}

void ItReturnsSameSharedInstanceWhileItIsAlive()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;

    RegisterSharedService(container, SameInstanceTestClass);

    int firstInstanceId;

    {
        auto instance1 = container.template GetRequiredService<SameInstanceTestClass>();
        auto instance2 = container.template GetRequiredService<SameInstanceTestClass>();

        assert(instance1->Id() == instance2->Id());

        firstInstanceId = instance1->Id();
    }

    auto instance3 = container.template GetRequiredService<SameInstanceTestClass>();
    assert(instance3->Id() != firstInstanceId);
}

void ItReturnsCorrectScopedServiceInstance()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;

    RegisterScopedService(container, SameInstanceTestClass);

    auto scope1 = container.CreateScope();
    auto scope2 = container.CreateScope();

    auto instance1 = scope1.template GetRequiredService<SameInstanceTestClass>();
    auto instance2 = scope1.template GetRequiredService<SameInstanceTestClass>();

    auto instance3 = scope2.template GetRequiredService<SameInstanceTestClass>();

    assert(instance1->Id() == instance2->Id());
    assert(instance1->Id() != instance3->Id());
}

void ItAllowsCreatingScopeOfAScope()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;
    RegisterScopedService(container, SameInstanceTestClass);

    auto scope1 = container.CreateScope();
    RegisterScopedService(scope1, SameInstanceTestClass);

    auto scope1_1 = scope1.CreateScope();

    auto instance1 = scope1.template GetRequiredService<SameInstanceTestClass>();
    auto instance1_1 = scope1_1.template GetRequiredService<SameInstanceTestClass>();

    assert(instance1->Id() != instance1_1->Id());
}

void ItReturnsMultipleRegisteredServices()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;

    RegisterSingletonService(container, SameInstanceTestClass, 1);
    RegisterSingletonService(container, SameInstanceTestClass, 2);
    RegisterSingletonService(container, SameInstanceTestClass, 3);

    auto instances = container.template GetServices<SameInstanceTestClass>();

    assert(instances[0]->Id() == 1);
    assert(instances[1]->Id() == 2);
    assert(instances[2]->Id() == 3);
}

void ItReturnsLastRegisteredService()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    Container container;

    RegisterSingletonService(container, SameInstanceTestClass, 1);
    RegisterSingletonService(container, SameInstanceTestClass, 2);
    RegisterSingletonService(container, SameInstanceTestClass, 3);

    auto service = container.template GetRequiredService<SameInstanceTestClass>();

    assert(service->Id() == 3);
}

void ItHandlesMultithreadedAccessCorrectly()
{
    using namespace sol::di::test;

    Container container;

    std::vector<std::thread> threads;

    for (int i = 0; i < 50; i++)
        threads.push_back(std::thread([&]() {
            for (int j = 0; j < 50; j++)
            {
                RegisterSingletonService(container, TestA);
                RegisterTransientService(container, TestB, FROM_DI(TestA));
                RegisterSharedService(container, TestC, FROM_DI(TestA), FROM_DI(TestB));
                RegisterSingletonInterface(container, ITestD, TestD, FROM_DI(TestC));
                RegisterTransientService(container, TestE, FROM_DI(ITestD));
                RegisterSharedService(container, TestF, FROM_DI(TestE));
                RegisterScopedService(container, TestG, FROM_DI_MULTIPLE(TestF));

                auto scope = container.CreateScope();

                auto d = container.template GetRequiredService<ITestD>();
                auto a = container.template GetRequiredService<TestA>();
                auto g = scope.template GetRequiredService<TestG>();
                auto e = container.template GetRequiredService<TestE>();
                auto c = container.template GetRequiredService<TestC>();
                auto b = container.template GetRequiredService<TestB>();
                auto f = container.template GetRequiredService<TestF>();

                assert(a != nullptr);
                assert(b != nullptr);
                assert(c != nullptr);
                assert(d != nullptr);
                assert(e != nullptr);
                assert(f != nullptr);
                assert(g != nullptr);
            }
        }));

    for (int i = 0; i < threads.size(); i++)
        if (threads[i].joinable())
            threads[i].join();
}

void ItDetectsCircularDependency()
{
    using namespace test;
    using namespace exc;

    Container container;

    RegisterSingletonService(
        container,
        CircularDependencyTestClassA,
        FROM_DI(CircularDependencyTestClassB)
    );

    RegisterSingletonService(
        container,
        CircularDependencyTestClassB,
        FROM_DI(CircularDependencyTestClassA)
    );

    RegisterSingletonService(
        container,
        CircularDependencyTestClassC,
        FROM_DI(CircularDependencyTestClassC)
    );

    bool exceptionThrownA = false;
    bool exceptionThrownB = false;
    bool exceptionThrownC = false;

    try
    {
        container.template GetRequiredService<CircularDependencyTestClassA>();
    }
    catch (const CircularDependencyException& ex)
    {
        exceptionThrownA = true;
    }

    try
    {
        container.template GetRequiredService<CircularDependencyTestClassB>();
    }
    catch (const CircularDependencyException& ex)
    {
        exceptionThrownB = true;
    }

    try
    {
        container.template GetRequiredService<CircularDependencyTestClassC>();
    }
    catch (const CircularDependencyException& ex)
    {
        exceptionThrownC = true;
    }

    assert(exceptionThrownA);
    assert(exceptionThrownB);
    assert(exceptionThrownC);
}

void RunTests()
{
    ItRegistersAndReturnsServices();
    ItReturnsSameSingletonInstance();
    ItRegistersSingletonByInstance();
    ItReturnsDifferentTransientInstances();
    ItReturnsSameSharedInstanceWhileItIsAlive();
    ItReturnsCorrectScopedServiceInstance();
    ItAllowsCreatingScopeOfAScope();
    ItReturnsMultipleRegisteredServices();
    ItReturnsLastRegisteredService();
    ItDetectsCircularDependency();

    ItHandlesMultithreadedAccessCorrectly();
}
