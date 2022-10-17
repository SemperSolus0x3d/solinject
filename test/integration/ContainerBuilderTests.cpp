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

void ItBuildsContainer()
{
    using namespace test;

    ContainerBuilder builder;

    builder.RegisterService<TestA>("TestA", FACTORY(TestA));
    builder.RegisterService<TestB>("TestB", FACTORY(TestB, FROM_DI(TestA)));
    builder.RegisterService<TestC>("TestC", FACTORY(TestC, FROM_DI(TestA), FROM_DI(TestB)));
    builder.RegisterInterface<ITestD>("ITestD");
    builder.RegisterService<TestD, ITestD>("TestD", FACTORY(TestD, FROM_DI(TestC)));

    Configuration configuration({
        ConfigurationItem("TestA", ServiceLifetime::Singleton),
        ConfigurationItem("TestB", ServiceLifetime::Transient),
        ConfigurationItem("TestC", ServiceLifetime::Shared),
        ConfigurationItem("ITestD", "TestD", ServiceLifetime::Singleton)
    });

    auto container = builder.BuildContainer(configuration);

    auto d = container.template GetRequiredService<ITestD>();
    auto a = container.template GetRequiredService<TestA>();
    auto c = container.template GetRequiredService<TestC>();
    auto b = container.template GetRequiredService<TestB>();

    assert(a != nullptr);
    assert(b != nullptr);
    assert(c != nullptr);
    assert(d != nullptr);
}

void ItHandlesMultipleImplementationsOfTheSameInterface()
{
    using namespace test;

    ContainerBuilder builder;

    builder.template RegisterInterface<ITestD>("ITestD");
    builder.template RegisterService<TestD, ITestD>("TestD", FACTORY(TestD, FROM_DI(TestC)));
    builder.template RegisterService<TestD2, ITestD>("TestD2", FACTORY(TestD2, FROM_DI(TestB)));
    builder.template RegisterService<TestH>("TestH", FACTORY(TestH, FROM_DI_MULTIPLE(ITestD)));
    builder.template RegisterService<TestA>("TestA", FACTORY(TestA));
    builder.template RegisterService<TestB>("TestB", FACTORY(TestB, FROM_DI(TestA)));
    builder.template RegisterService<TestC>("TestC", FACTORY(TestC,FROM_DI(TestA), FROM_DI(TestB)));

    Configuration configuration({
        ConfigurationItem("ITestD", "TestD", ServiceLifetime::Singleton),
        ConfigurationItem("ITestD", "TestD2", ServiceLifetime::Transient),
        ConfigurationItem("TestH", ServiceLifetime::Singleton),
        ConfigurationItem("TestA", ServiceLifetime::Singleton),
        ConfigurationItem("TestB", ServiceLifetime::Singleton),
        ConfigurationItem("TestC", ServiceLifetime::Singleton)
    });

    auto container = builder.BuildContainer(configuration);

    auto ds = container.template GetServices<ITestD>();

    assert(ds.size() == 2);
    assert(ds[0] != nullptr);
    assert(ds[1] != nullptr);
}

void ItHandlesInterfaceToInterfaceRegistration()
{
    using namespace test;

    ContainerBuilder builder;

    builder.template RegisterInterface<ITestD>("ITestD");
    builder.template RegisterInterface<ITestD2>("ITestD2");
    builder.template RegisterService<TestD3, ITestD2, ITestD>("TestD3", FACTORY(TestD3));

    Configuration configuration({
        ConfigurationItem("ITestD", "ITestD2", ServiceLifetime::None),
        ConfigurationItem("ITestD2", "TestD3", ServiceLifetime::None),
        ConfigurationItem("TestD3", ServiceLifetime::Singleton)
    });

    auto container = builder.BuildContainer(configuration);

    auto id = container.template GetRequiredService<ITestD>();
    auto id2 = container.template GetRequiredService<ITestD2>();
    auto d3 = container.template GetRequiredService<TestD3>();

    assert(id == id2);
    assert(id2 == d3);
    assert(d3 != nullptr);
}

void RunTests()
{
    ItBuildsContainer();
    ItHandlesMultipleImplementationsOfTheSameInterface();
    ItHandlesInterfaceToInterfaceRegistration();
}
