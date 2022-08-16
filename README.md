# solinject

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=flat&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat&logo=cmake&logoColor=white)
![OS: Windows](https://img.shields.io/static/v1?label=OS&message=Windows&color=green&style=flat)
![OS: Linux](https://img.shields.io/static/v1?label=OS&message=Linux&color=green&style=flat)

C++17 Dependency Injection header-only library

## Features

- Supports singleton[^singleton-service], transient[^transient-service], shared[^shared-service] and scoped[^scoped-service] services.
- Supports registering multiple services of the same type or multiple implementations of the same interface and resolving them all at once.
- Threadsafe (can be disabled if your program is single-threaded).
- Has runtime circular dependency checks.
- Works on Windows and Linux. Other platforms support is untested.
- You don't have to change your services in order to use them with solinject (except that they should accept and use their dependencies as [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr)).
- Your services don't have to know about their dependencies' lifetime.
- All configuration is tied to the container.
- Container is mutable by default, but it will be immutable if you use it by a reference to const or by a pointer to const.

## How to use it

### `#include` it

```c++
#define SOLINJECT_NOTHREADSAFE // If your program is single-threaded
#include <solinject.hpp>
#include <solinject-macros.hpp>
```

`solinject.hpp` contains the `sol::di::DIContainer` class, and `solinject-macros.hpp` provides you some handy macros for registering services.

### Create a container

```c++
sol::di::DIContainer container;
```

### Register a service

```c++
RegisterSingletonService(container, MyServiceClass/*, constructor params go here */);
// or
RegisterSingletonInterface(container, IMyServiceInterface, MyServiceClass/*, constructor params go here */);
```

If your service has constructor parameters, that should be injected from the container, use the `FROM_DI()` macro or the `FROM_DI_MULTIPLE()` macro:

```c++
RegisterSingletonService(container, MyServiceClass, FROM_DI(MyOtherServiceClass));
// or
RegisterSingletonInterface(container, IMyServiceInterface, MyServiceClass, FROM_DI(MyOtherServiceClass));
```

The `FROM_DI()` macro injects a single instance of the service as `std::shared_ptr<T>`, while the `FROM_DI_MULTIPLE()` macro injects multiple instances of the service or multiple implementations of the interface as `std::vector<std::shared_ptr<T>>`.

If you have a [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr) or a [`std::unique_ptr<>`](https://en.cppreference.com/w/cpp/memory/unique_ptr) to an instance of the service, you can register it as a singleton:

```c++
auto instance1 = std::make_shared<MyServiceClass>();
auto instance2 = std::make_unique<MyOtherServiceClass>();

container.template RegisterSingletonService<MyServiceClass>(instance1);
container.template RegisterSingletonService<MyOtherServiceClass>(std::move(instance2));
```

If for some reason you want to go the hard way, you can register the service directly using a lambda expression:

```c++
container.template RegisterSingletonService<MyServiceClass>([](const auto& container)
{
    return std::make_shared<MyServiceClass>(
        container.template GetRequiredService<MyOtherServiceClass>()
    );
});
// or
container.template RegisterSingletonService<IMyServiceInterface>([](const auto& container)
{
    return std::make_shared<MyServiceClass>(
        container.template GetRequiredService<MyOtherServiceClass>()
    );
});
```

### Get the service from the container

```c++
// For a single instance:
std::shared_ptr<MyServiceClass> myService = container.template GetRequiredService<MyServiceClass>();
// or
std::shared_ptr<IMyServiceInterface> myService = container.template GetRequiredService<IMyServiceInterface>();

// For multiple instances or implementations:
std::vector<std::shared_ptr<MyServiceClass>> myServices = container.template GetServices<MyServiceClass>();
// or
std::vector<std::shared_ptr<IMyServiceInterface>> myServices = container.template GetServices<IMyServiceInterface>();
```

The `GetRequiredService<>()` method will throw `sol::di::exceptions::ServiceNotRegisteredException` if the requested service is not registered. If you prefer to get an empty [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr) in such cases, use the `GetService<>()` method.

If you want to use scoped services, then create a scope:

```c++
sol::di::DIContainerScope scope = container.CreateScope();
```

and resolve your services from this scope just like you would from a container:

```c++
std::shared_ptr<MyServiceClass> myService1 = scope.template GetRequiredService<MyServiceClass>();

std::shared_ptr<IMyServiceInterface> myService2 = scope.template GetRequiredService<IMyServiceInterface>();

std::vector<std::shared_ptr<MyServiceClass>> myServices1 = scope.template GetServices<MyServiceClass>();

std::vector<std::shared_ptr<IMyServiceInterface>> myServices2 = scope.template GetServices<IMyServiceInterface>();
```

`sol::di::DIContainerScope` can do everything a regular `sol::di::DIContainer` can do. You can register services to it (even scoped services), resolve services from it etc. You can even create a scope from a scope, and then create a scope from that scope and so on.

## How to link it to your project

There are a few ways to link solinject to your project. Here's an incomplete list of them (the most recommended first):

- ### CMake FetchContent

    Add these lines to your top-level `CMakeLists.txt`, replacing `v1.0.0` with your preferred version:

    ```cmake
    include(FetchContent)

    FetchContent_Declare(
        solinject
        GIT_REPOSITORY https://github.com/SemperSolus0x3d/solinject
        GIT_TAG v1.0.0
    )

    FetchContent_MakeAvailable(solinject)
    ```

    And then link the `sol::solinject` target to your project's target:

    ```cmake
    target_link_libraries(YOUR_TARGET sol::solinject)
    ```

- ### Git Submodule

    In your project folder run:

    ```bash
    mkdir extern
    git submodule add "https://github.com/SemperSolus0x3d/solinject" extern/solinject
    cd extern/solinject
    git checkout v1.0.0
    cd ../..
    ```

    Then in your top-level `CMakeLists.txt` add this submodule as subdirectory and link `sol::solinject` to your project's target:

    ```cmake
    add_subdirectory(extern/solinject)
    target_link_libraries(YOUR_TARGET sol::solinject)
    ```

- ### CMake find_package()

    [Build](#how-to-build-it) or download the [latest release](https://github.com/SemperSolus0x3d/solinject/releases/latest), install it to your CMake packages installation prefix and link it to your project's target:

    ```cmake
    find_package(solinject VERSION 1.0.0 REQUIRED)
    target_link_libraries(YOUR_TARGET sol::solinject)
    ```

- ### Add as subdirectory

    Download the [latest release](https://github.com/SemperSolus0x3d/solinject/releases/latest) source code, extract it to your project's directory for dependencies (in my example it's `lib`) and add it to your project as subdirectory:

    ```cmake
    add_subdirectory(lib/solinject)
    target_link_libraries(YOUR_TARGET sol::solinject)
    ```

- ### Copy-paste

    solinject is a header-only library, so you can just copy everything from this project's `include` directory and paste it into your project's `include` directory. Make sure to add the folder containing `solinject.hpp` and `solinject-macros.hpp` to your compiler include directories.

## How to build it

solinject is a header-only library, so building it is needed only to build tests and install a cmake package, which then can be linked with `find_package()`.

Clone the repository and `cd` into it:

```bash
git clone https://github.com/SemperSolus0x3d/solinject/
cd solinject
```

At this point you can use the `BuildForAllPlatformsVS.py` script for the Visual Studio generator:

```bash
scripts/BuildForAllPlatformsVS.py
```

or `BuildForAllPlatformsUnixMakefiles.py` for the Unix Makefiles generator:

```bash
scripts/BuildForAllPlatformsUnixMakefiles.py
```

or just build the project manually for every platform you need:

```bash
cmake -S . -B build -DBUILD_TESTING=0
cmake --build build
cmake --install build --prefix install
```

The way how you should tell cmake the platform you want depends on the generator you are using.

## License

SPDX-License-Identifier: LGPL-3.0-or-later

Copyright (C) 2022  SemperSolus0x3d

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

[^singleton-service]: A singleton service is created once and used everywhere.

[^transient-service]: A transient service is created each time it is requested.

[^shared-service]: A shared service exists while it is used. When a shared service is requested, if an instance already exists, that instance is returned; otherwise a new instance is created.

[^scoped-service]: A scoped service behaves like a singleton inside its scope and derived scopes.
