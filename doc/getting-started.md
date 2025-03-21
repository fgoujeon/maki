# Getting Started {#getting-started}

## Requirements

You need a properly implemented C++17 compiler.

Maki is tested with these compilers:

* Visual Studio 2022;
* GCC 10, 11 and 12;
* Clang 13, 14 and 15.

> [!important]
> Building Maki tests and examples causes Visual Studio 2017 and 2019 to fail with internal compiler errors.

## Installation and Usage

Maki being a header-only library, you don't have to build it.

To install it into your project, you can either:

* use the [GitHub repository](https://github.com/fgoujeon/maki) as a git submodule and select a tagged revision on the `main` branch;
* download the latest [tagged revision](https://github.com/fgoujeon/maki/tags) of the `main` branch and copy the `maki` subdirectory wherever suits you.

If you use CMake, you'd likely want to `add_subdirectory()` the `maki` subdirectory. Its `CMakeLists.txt` script doesn't depend on anything. It defines a `maki` target that you can use like so:

```cmake
target_link_libraries(MyExecutable PRIVATE maki)
```

You can then `#include` Maki headers from your `MyExecutable` program and start using the library.
