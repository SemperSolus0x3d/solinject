# solinject

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=flat&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=flat&logo=cmake&logoColor=white)
![OS: Windows](https://img.shields.io/static/v1?label=OS&message=Windows&color=green&style=flat)
![OS: Linux](https://img.shields.io/static/v1?label=OS&message=Linux&color=green&style=flat)

C++17 Dependency Injection header-only library

## Features

- Supports singleton[^singleton-service], transient[^transient-service] and shared[^shared_service] services.
- Threadsafe (can be disabled if your program is single-threaded).
- Works on Windows and Linux. Other platforms support is untested.
- You don't have to change your services in order to use them with solinject (except that they should accept and use their dependencies as [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr)).
- Your services don't have to know about their dependencies' lifetime.
- All configuration is tied to the container.
- Container is mutable by default, but it will be immutable if you use it by a reference to const or by a pointer to const.

## How to `#include` it

Depending on the way you link it to your project, you will have to `#include` it as

```c++
#include <solinject.hpp>
#include <solinject-macros.hpp>
```

or

```c++
#include <solinject/solinject.hpp>
#include <solinject/solinject-macros.hpp>
```

`solinject.hpp` contains the `sol::di::DIContainer` class, and `solinject-macros.hpp` provides you some handy macros for registering services.

## How to use it

### Create a container

```c++
sol::di::DIContainer<> container;
```

### Register a service

```c++
RegisterSingletonService(container, MyServiceClass/*, constructor params go here */);
// or
RegisterSingletonInterface(container, IMyServiceInterface, MyServiceClass/*, constructor params go here */);
```

If your service has constructor parameters, that should be injected from the container, use the `FROM_DI()` macro:

```c++
RegisterSingletonService(container, MyServiceClass, FROM_DI(MyOtherServiceClass));
// or
RegisterSingletonInterface(container, IMyServiceInterface, MyServiceClass, FROM_DI(MyOtherServiceClass));
```

If for some reason you want to go the hard way, you can register the service directly using a lambda expression:

```c++
container.template RegisterSingletonService<MyServiceClass>([](const auto& container)
{
    return std::make_unique<MyServiceClass>(
        container.template GetRequiredService<MyOtherServiceClass>()
    );
});
// or
container.template RegisterSingletonService<IMyServiceInterface>([](const auto& container)
{
    return std::make_unique<MyServiceClass>(
        container.template GetRequiredService<MyOtherServiceClass>()
    );
});
```

### Get the service from the container

```c++
std::shared_ptr<MyServiceClass> myService = container.template GetRequiredServicePtr<MyServiceClass>();
// or
std::shared_ptr<IMyServiceInterface> myService = container.template GetRequiredServicePtr<IMyServiceInterface>();
```

The `GetRequiredServicePtr<>()` method will throw [`std::out_of_range`](https://en.cppreference.com/w/cpp/error/out_of_range) if the requested service is not registered. If you prefer to get an empty [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr) in such cases, use the `GetServicePtr<>()` method.

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

solinject is a header-only library, so you can just copy everything from project's `include` directory and paste it into your project's `include` directory

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

[^shared_service]: A shared service exists while it is used. When a shared service is requested, if an instance already exists, that instance is returned; otherwise a new instance is created.
