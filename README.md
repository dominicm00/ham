# Ham - WIP {#index}

## Disclaimer
This repository holds Ham v1, based on Ingo Weinhold's original work, and is in _maintenance mode_. Bug fixes and quality-of-life changes are still being merged, but new features are not being added.

Development is currently underway on Ham v2, which is a heavily rewritten version with better parsing, exact header scanning, improved incremental builds, and more. You can check it out [here](https://sr.ht/~dominicm/ham/).

## Known bugs
There are currently command execution bugs blocking the build for [Haiku](https://www.haiku-os.org/) that are being addressed. Although Ham currently works for some smaller builds, until Ham is stress-tested on Haiku and other Jam projects, it is not recommended to be used in production.

## Introduction
Ham is a drop-in replacement for the [Jam build system](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html). Its primary goal is to be compatible with and extend [Haiku Jam](https://git.haiku-os.org/buildtools/tree/jam), and eventually become Haiku's official build system. However, compatibility with [Perforce Jam](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html), [Boost.Build](https://www.boost.org/build), and possibly other variants are planned.

## Motivation
While Jam is a great build system, its legacy codebase makes it difficult to fix bugs or introduce new features. Ham uses a modern, friendly C++ codebase, and is rigorously tested. Ham also has more robust multithreading support, and is written as a library for IDE integration:

## Usage
To run Ham in compatibility mode, use the `-c/--compat` flag. For instance, you can define `jam` with an alias:

```sh
alias jam='ham --compat'
```

While in compatibility mode, Ham will attempt to respect the command line parameters and features of the chosen tool. For convenience, Ham provides aliases for many commands. A compatibility matrix is provided below:

*Key: X=implemented, P=planned, N/A=not applicable*

| Flags                | Description                    | `ham` | `jam`     | `ham --compat` |
|:---------------------|:-------------------------------|:-----:|:---------:|:--------------:|
| `-a`                 | build all                      | P     | **X**     | P              |
| `-dX`                | debug info                     | P[^1] | **X**     | P              |
| `-fX`                | select ruleset                 | P     | **X**     | P              |
| `-g`                 | new sources first              | P     | **X**     | P              |
| `-jX`                | run actions concurrently       | P     | **X[^2]** | P              |
| `-n`                 | dry run                        | P     | **X**     | P              |
| `-oX`                | output commands to file        | P     | **X**     | P              |
| `-q`                 | quit on build failure          | P     | **X**     | P              |
| `-sX=Y`              | set variable                   | P     | **X**     | P              |
| `-tX`                | rebuild target                 | P     | **X**     | P              |

Info about the Jam language can be found in the [Perforce documentation](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html) and the (archived) [Gentoo tutorial](https://web.archive.org/web/20160304233139/http://geoz.co.nz/jamdoc/jam-guide.html). 

## Building Ham
Ham is built using [Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html). Currently, no distribution tarballs are available, so Ham must be built from the repository source code.

To build Ham from this repository, the following dependencies are required:

- [Autoconf](https://www.gnu.org/software/autoconf/)
- [Automake](https://www.gnu.org/software/automake/)
- [Libtool](https://www.gnu.org/software/libtool/)
- A C++ compiler with support for C++20

To build Ham from the repository source code, initialize the Automake system and then run the standard `./configure && make`.

```sh
autoreconf --install
./configure
# Add -jN (where N is the number of parallel CPUs on your system) for faster builds
make
```

You can then run `make install` to install Ham on your system.

## Contributing
To contribute to Ham, you'll need:
- `clang-format` v14 or above

Before submitting a pull request, make sure to run the `format.sh` script.

[^1]: Debug information/levels may differ from Jam.
[^2]: `jam`'s implementation of concurrent actions has known bugs.
