# Ham - WIP {#index}

## Disclaimer
Ham is a work-in-progress, and currently **does not work**. Please do not attempt to use Ham other than for experimental/development purposes.

This branch is an in-progress rewrite of the original Ham project. The rationale behind the rewrite is in [ADR #8](docs/development/decisions/0008-creating-a-version-2.md). The original code is in the `legacy` branch.

## Introduction
Ham is a replacement for the [Jam build system](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html). Its primary goal is to replace [Haiku Jam](https://git.haiku-os.org/buildtools/tree/jam), as Haiku's official build system. However, any idiomatic Jam project should also work with Ham.

## Motivation
Jam's legacy codebase makes it difficult to fix bugs or introduce new features. Ham uses a modern, friendly C++ codebase, is rigorously tested, and provides much better debugging output. Backed by [Ninja](https://ninja-build.org/), it has fast incremental build times, exact header dependency scanning, and more. Detailed project goals can be found in the [goals document](docs/development/decisions/0009-ham-project-goals.md).

## Usage
Ham's command line input is undergoing design.

## Building Ham
Ham is built using [CMake](https://cmake.org/). Currently, no distribution packages are available, so Ham must be built from the repository source code.

To build Ham from this repository, you will need a compiler supporting C++20 or higher. [Ninja](https://ninja-build.org/) is not required to build Ham, but it is required to run it.

**NOTE:** Ham plans to take the following dependencies in the near future:
- [Flex](https://github.com/westes/flex) (not required with distribution tarball)
- [Bison](https://www.gnu.org/software/bison/) (not required with distribution tarball)

Ham follows standard conventions for a CMake project; example build commands are provided below for convenience.
```sh
mkdir build && cd build
cmake ..
cmake --build .
```

## Contributing
To contribute to Ham, you'll need:
- `clang-format` v14 or above

Before submitting a pull request, make sure to run the `format.sh` script.
