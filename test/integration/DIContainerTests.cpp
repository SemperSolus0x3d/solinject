#include <iostream>
#include <solinject.hpp>
#include <solinject-macros.hpp>

sol::di::DIContainer RegisterServices();

class A {};

int main()
{
    auto container = RegisterServices();

    try
    {
        container.GetRequiredServicePtr<A>();
    }
    catch (std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        return -1;
    }
    return 0;
}

sol::di::DIContainer RegisterServices()
{
    sol::di::DIContainer container;

    RegisterSingletonService(container, A);

    return container;
}