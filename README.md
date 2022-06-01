# Ham - WIP

## Disclaimer
Ham is a work-in-progress, and currently **does not work**. Please do not
attempt to use Ham other than for experimental/development purposes.

## Introduction
Ham is a drop-in replacement for the [Jam build
system](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html). It's
primary goal is to be compatible with and extend [Haiku
Jam](https://git.haiku-os.org/buildtools/tree/jam), and eventually be used as
Haiku's official build system. However, compatibility with [Perforce
Jam](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html),
[Boost.Build](https://www.boost.org/build), and possibly other variants is
planned.

## Motivation
While Jam is a great build system, its legacy codebase makes it difficult to fix
bugs or introduce new features. Ham uses a modern, friendly C++ codebase, is
rigorously tested, and combines the various forks of Jam into one tool. Ham will
also have additional features such as:
- More robust multi-threading support, fixing long-standing bugs when using
  `-jN`.
- A granular caching system that avoids large rebuilds when changing
  Jamfiles/environment variables.
- Written as a library for IDE integration.

## Usage
**Disclaimer:** This represents the planned interface for Ham, but currently
does not work.

To run Ham in compatability mode, use the `-cX` flag with the value `jam` for
[Perforce
Jam](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html),
`boost` for [Boost.Build](https://www.boost.org/build), or `ham` (the
default). For example, you can define `jam` and `b2` as follows:

```shell
alias jam='ham -cjam'
alias b2='ham -cboost'
```

While in compatibility mode, Ham will attempt to respect the command line
parameters and features of the chosen tool. For convienence, Ham provides
aliases for many commands. A compatability matrix is provided below:

*Key: X=implemented, P=planned, N/A=not applicable*

| Flags                | Description                    | `ham` | `jam`     | `ham -cjam` | `b2`  | `ham -cboost` |
|:---------------------|:-------------------------------|:-----:|:---------:|:-----------:|:-----:|:-------------:|
| `-a`                 | build all                      | P     | **X**     | P           | **X** | P             |
| `-dX`                | debug info                     | P[^1] | **X**     | P[^1]       | **X** | P[^1]         |
| `-fX`                | select Jambase                 | P     | **X**     | P           | **X** | P             |
| `-g`                 | new sources first              | P     | **X**     | P           | **X** | P             |
| `-jX`                | run actions concurrently       | P     | **X[^2]** | P           | **X** | P             |
| `-n`                 | dry run                        | P     | **X**     | P           | **X** | P             |
| `-oX`                | output commands to file        | P     | **X**     | P           | **X** | P             |
| `-q`                 | quit on build failure          | P[^3] | **X**     | P           | **X** | P             |
| `-c`                 | continue on build failure      | P     | *N/A*     | *N/A*       | *N/A* | *N/A*         |
| `-sX=Y`              | set variable                   | P     | **X**     | P           | **X** | P             |
| `-tX`                | rebuild target                 | P     | **X**     | P           | **X** | P             |
| `--x`                | forward args to scripts        | P     |           |             | **X** | P             |
| `--abbreviate-paths` | abbreviate autogenerated paths | *N/A* | *N/A*     | *N/A*       | **X** | P             |
| `--hash`             | hash autogenerated paths       | *N/A* | *N/A*     | *N/A*       | **X** | P             |
| `-dconsole`          | run interactive debugger       |       |           |             | **X** |               |
| `-e`                 | export compilation database    | P     |           | P           |       | P             |

Info about the Jam language can be found in the [Perforce
documentation](https://swarm.workshop.perforce.com/view/guest/perforce_software/jam/src/Jam.html)
and the (archived) [Gentoo
tutorial](https://web.archive.org/web/20160304233139/http://geoz.co.nz/jamdoc/jam-guide.html). Documentation
of Ham's built-in rules is currently in-progress.

## Building Ham
Ham is currently only tested to build on Linux, and can be built by running
`Bootstrap.sh`. Work is ongoing to bootstrap off the GNU build system for a more
portable building experience.

[^1]: Debug information/levels differ between `ham`, `jam`, and `b2`.
[^2]: `jam` has several bugs with their implementation of concurrent actions.
[^3]: In `-cham` mode, `-q` is enabled by default, but the flag is accepted for
    convienence.
