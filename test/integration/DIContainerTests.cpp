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

template<bool isThreadsafe>
void RunTests();

int main()
{
    try
    {
        RunTests<true>();
        RunTests<false>();

        return 0;
    }
    catch (std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return -1;
}

template <bool isThreadsafe>
void ItRegistersAndReturnsServices()
{
    using namespace sol::di::test;

    DIContainer<isThreadsafe> container;

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

template <bool isThreadsafe>
void ItReturnsSameSingletonInstance()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    DIContainer<isThreadsafe> container;

    RegisterSingletonService(container, SameInstanceTestClass);

    auto instance1 = container.template GetRequiredService<SameInstanceTestClass>();
    auto instance2 = container.template GetRequiredService<SameInstanceTestClass>();

    assert(instance1->Id() == instance2->Id());
}

template <bool isThreadsafe>
void ItReturnsDifferentTransientInstances()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    DIContainer<isThreadsafe> container;

    RegisterTransientService(container, SameInstanceTestClass);

    auto instance1 = container.template GetRequiredService<SameInstanceTestClass>();
    auto instance2 = container.template GetRequiredService<SameInstanceTestClass>();

    assert(instance1->Id() != instance2->Id());
}

template <bool isThreadsafe>
void ItReturnsSameSharedInstanceWhileItIsAlive()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    DIContainer<isThreadsafe> container;

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

template <bool isThreadsafe>
void ItReturnsMultipleRegisteredServices()
{
    using namespace test;

    DIContainer<isThreadsafe> container;

    RegisterSingletonService(container, SameInstanceTestClass, 1);
    RegisterSingletonService(container, SameInstanceTestClass, 2);
    RegisterSingletonService(container, SameInstanceTestClass, 3);

    auto instances = container.template GetServices<SameInstanceTestClass>();

    assert(instances[0]->Id() == 1);
    assert(instances[1]->Id() == 2);
    assert(instances[2]->Id() == 3);
}

template <bool isThreadsafe>
void ItReturnsLastRegisteredService()
{
    using namespace test;

    DIContainer<isThreadsafe> container;

    RegisterSingletonService(container, SameInstanceTestClass, 1);
    RegisterSingletonService(container, SameInstanceTestClass, 2);
    RegisterSingletonService(container, SameInstanceTestClass, 3);

    auto service = container.template GetRequiredService<SameInstanceTestClass>();

    assert(service->Id() == 3);
}

void ItHandlesMultithreadedAccessCorrectly()
{
    using namespace sol::di::test;

    DIContainer<true> container;

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
        }));

    for (int i = 0; i < threads.size(); i++)
        if (threads[i].joinable())
            threads[i].join();
}

template<bool isThreadsafe>
void RunTests()
{
    ItRegistersAndReturnsServices<isThreadsafe>();
    ItReturnsSameSingletonInstance<isThreadsafe>();
    ItReturnsDifferentTransientInstances<isThreadsafe>();
    ItReturnsSameSharedInstanceWhileItIsAlive<isThreadsafe>();
    ItReturnsMultipleRegisteredServices<isThreadsafe>();
    ItReturnsLastRegisteredService<isThreadsafe>();

    if constexpr (isThreadsafe)
    {
        ItHandlesMultithreadedAccessCorrectly();
    }
}
