#include <iostream>
#include <vector>
#include <thread>
#include <assert.h>
#include <solinject.hpp>
#include <solinject-macros.hpp>

#include "TestClasses.h"

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
    RegisterSingletonService(container, TestC, FROM_DI(TestA), FROM_DI(TestB));
    RegisterTransientInterface(container, ITestD, TestD, FROM_DI(TestC));

    auto d = container.GetRequiredServicePtr<ITestD>();
    auto a = container.GetRequiredServicePtr<TestA>();
    auto c = container.GetRequiredServicePtr<TestC>();
    auto b = container.GetRequiredServicePtr<TestB>();

    assert(a != nullptr);
    assert(b != nullptr);
    assert(c != nullptr);
    assert(d != nullptr);
}

template <bool isThreadsafe>
void ItReturnsSameSingletonInstance()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    DIContainer<isThreadsafe> container;

    RegisterSingletonService(container, SameInstanceTestClass);

    auto instance1 = container.GetRequiredServicePtr<SameInstanceTestClass>();
    auto instance2 = container.GetRequiredServicePtr<SameInstanceTestClass>();

    assert(instance1->Id() == instance2->Id());
}

template <bool isThreadsafe>
void ItReturnsDifferentTransientInstances()
{
    using namespace test;

    SameInstanceTestClass::ResetIds();

    DIContainer<isThreadsafe> container;

    RegisterTransientService(container, SameInstanceTestClass);

    auto instance1 = container.GetRequiredServicePtr<SameInstanceTestClass>();
    auto instance2 = container.GetRequiredServicePtr<SameInstanceTestClass>();

    assert(instance1->Id() != instance2->Id());
}

void ItHandlesMultithreadedAccessCorrectly()
{
    using namespace sol::di::test;

    DIContainer<true> container;

    std::vector<std::thread> threads;

    for (int i = 0; i < 100; i++)
        threads.push_back(std::thread([&]() {
            for (int j = 0; j < 1000; j++)
            {
                RegisterSingletonService(container, TestA);
                RegisterTransientService(container, TestB, FROM_DI(TestA));
                RegisterSingletonService(container, TestC, FROM_DI(TestA), FROM_DI(TestB));
                RegisterTransientInterface(container, ITestD, TestD, FROM_DI(TestC));

                auto d = container.GetRequiredServicePtr<ITestD>();
                auto a = container.GetRequiredServicePtr<TestA>();
                auto c = container.GetRequiredServicePtr<TestC>();
                auto b = container.GetRequiredServicePtr<TestB>();

                assert(a != nullptr);
                assert(b != nullptr);
                assert(c != nullptr);
                assert(d != nullptr);
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

    if constexpr (isThreadsafe)
    {
        ItHandlesMultithreadedAccessCorrectly();
    }
}
