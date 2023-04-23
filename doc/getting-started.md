# Getting Started {#getting-started}

AweSM being a header-only library, you don't have to build it.

To install it into your project, you can either:

* use the [GitHub repository](https://github.com/fgoujeon/awesm) as a git submodule and select a tagged revision on the `master` branch;
* download the latest [tagged revision](https://github.com/fgoujeon/awesm/tags) of the `master` branch and copy the `awesm` subdirectory wherever suits you.

If you use CMake, you'd likely want to `add_subdirectory()` the `awesm` subdirectory. Its `CMakeLists.txt` script doesn't depend on anything. It defines an `awesm` target that you can use like so:

```cmake
target_link_libraries(MyExecutable PRIVATE awesm)
```

You can then `#include` AweSM headers from your `MyExecutable` program and start using the library.
