# Getting Started {#getting-started}

Maki being a header-only library, you don't have to build it.

To install it into your project, you can either:

* use the [GitHub repository](https://github.com/fgoujeon/maki) as a git submodule and select a tagged revision on the `master` branch;
* download the latest [tagged revision](https://github.com/fgoujeon/maki/tags) of the `master` branch and copy the `maki` subdirectory wherever suits you.

If you use CMake, you'd likely want to `add_subdirectory()` the `maki` subdirectory. Its `CMakeLists.txt` script doesn't depend on anything. It defines an `maki` target that you can use like so:

```cmake
target_link_libraries(MyExecutable PRIVATE maki)
```

You can then `#include` Maki headers from your `MyExecutable` program and start using the library.
