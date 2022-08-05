# 8. Creating a version 2
Date: 2022-07-29

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Status](#status)
- [Context](#context)
  - [Current architectural problems](#current-architectural-problems)
    - [Parsing](#parsing)
    - [Testing](#testing)
    - [Containers](#containers)
    - [AST](#ast)
    - [Caching](#caching)
    - [Dependency Detection](#dependency-detection)
    - [Jam Compatibility](#jam-compatibility)
    - [Platform Compatibility](#platform-compatibility)
    - [Lack of clear user-facing improvements](#lack-of-clear-user-facing-improvements)
  - [Approaches moving forward](#approaches-moving-forward)
    - [Keep current Ham architecture](#keep-current-ham-architecture)
    - [Change Ham's architecture](#change-hams-architecture)
      - [Use standard library, but no dependencies](#use-standard-library-but-no-dependencies)
      - [Use dependencies, but no standard library](#use-dependencies-but-no-standard-library)
      - [Use dependencies and standard library](#use-dependencies-and-standard-library)
    - [Change method](#change-method)
      - [Change gradually](#change-gradually)
      - [Change all at once](#change-all-at-once)
  - [Constraining factors](#constraining-factors)
    - [GSoC timeline](#gsoc-timeline)
    - [Reliability](#reliability)
- [Decision](#decision)
- [Consequences](#consequences)

<!-- markdown-toc end -->

## Status
Accepted

## Context

### Current architectural problems
#### Parsing
Ham implements a custom parser. This makes it difficult to change Ham's grammar, and adds more surface area for maintenance. The parser architecture also makes it difficult to unit test parsing rules, and there are currently no tests for the parser.

Ham's parser is architectured around tokenizing on whitespace, and the Jam language was, on paper, designed to be parsed this way. However, since the parser acts on tokens, it limits rule granularity. For instance, variables essentially implement a mini-parser in their evaluation function, since the overall parser can only act on the token level. It also means all new constructs have to be separated by whitespace.

#### Testing
Ham implements its own test framework. This means Ham's tests can't integrate with common tooling/CI, and also that many useful features in modern test frameworks are unavailable.

Ham's current test suite relies primarily on integration testing, run with a semi-unfinished test file structure. These tests are opaque to outside developers, and have a learning curve to write. It can also be difficult to narrow regressions to a small part of the code.

#### Containers
Ham implements its own containers, making it difficult to interface with outside code, including the standard library. These containers have inconsistent interfaces (e.x. `Acquire()/AcquireReference()`, different iterators) that cause internal fragmentation. They also perform manual memory management, require manual reference counting, and don't provide the type-level safety many standard library containers do, making ownership unclear.

#### AST
Ham AST nodes don't retain source code information, making it difficult to provide accurate errors.

#### Caching
Ham currently has no infrastructure to support caching the command line invocations that build targets. This means that if there's a change to environment variables/Jamfiles, Ham cannot reliably rebuild the affected items. This necessitates a full rebuild on any change to the invoked commands, which adds up in a project like Haiku.

#### Dependency Detection
Ham detects header dependencies by globbing each source file. This is quite slow and doesn't take conditional macros into account, leading to longer evaluation stages and unnecessary rebuilds.

#### Jam Compatibility
A stated goal of Ham is bug-for-bug compatibility with Perforce Jam (and maybe Boost.Build). This has proven unrealistic; Jam is loose with its language semantics, and doesn't always provide clearly reproducible output (see [#50](https://github.com/dominicm00/ham/pull/50#issuecomment-1170568688) for an example). It's also questionable if Ham _should_ be trying to replicate Jam's bugs.

#### Platform Compatibility
Ham's handling of paths/file access needs to be refactored to be compatible with non-Unix systems like Windows.

#### Lack of clear user-facing improvements
Ham's main goal is to be a better-architectured version of Jam to make future development easier. However, other than fixing bugs, Ham doesn't have clear targets for how it will provide user-facing improvements. Additionally, because of the limitation of being bug-for-bug compatible with Jam, Ham's behavior is necessarily similar to Jam's.

### Approaches moving forward
#### Keep current Ham architecture
Keeping the current architecture and fixing issues is the most straightforward option. Things that would have to be fixed for Ham to be considered production ready would be:
- Expanding the testing infrastructure to cover parsing and command line input.
- Adding (and refactoring around) unit tests.
- Improving platform compatibility.

#### Change Ham's architecture
##### Use standard library, but no dependencies
Using the standard library has many benefits in terms of implementation reliability, and being consistent with the C++ ecosystem. That said, there are valid complaints about C++ STL APIs, which can be difficult to use. Ham's data structures are also deeply embedded, making it difficult to remove them from all of Ham's (decently large) codebase.

##### Use dependencies, but no standard library
This requires maintaining boundaries between dependencies (which use the C++ STL) and Ham containers. Ham itself may also expose C++ STL types in its library API. Maintaining a boundary between internal/external types is difficult and inefficient. Besides, given the project would need to be refactored anyway to use the dependencies, it doesn't make much sense to avoid the standard library.

##### Use dependencies and standard library
Adding dependencies has the following benefits:
- Use trusted implementations
- Greatly eases work for platform compatibility
- Makes cross-cutting changes easier
- Adds new features (like debugging output) that could be exposed to users

And downsides:
- Increases build complexity
- Possibility of external breakage
- Possibility of abandoned dependencies

#### Change method
There are two ways to change Ham's architecture.

##### Change gradually
Ham can gradually modify its architecture, maintaining a boundary between old/new containers. This requires more work since each piece of code would likely be changed multiple times over the course of the refactor, but would allow Ham to be used during the refactor process. That said, there would likely be instability during the refactor.

##### Change all at once
Ham can change its entire architecture at once. This is theoretically less work, but can be slow if changes are not done atomically and tested properly. This also prevents Ham from being used during the refactor.

### Constraining factors
#### GSoC timeline
At this point, there is about 1 month left in GSoC. Considering I will be in school, large changes are likely not possible past this period. The design of Ham, whatever it is, will be stable after this point. By September, Ham should be production ready, or only need bug fixes/testing to be production ready.

#### Reliability
For Haiku to transition to Ham, it needs to be reliable, both in terms of behavioral stability and working well into the future. Ham needs to be a production-ready tool with a solid maintenance plan.

## Decision
Ham will do a v2 refactor to migrate to a new parser, test framework, and build system backend.

Changing gradually is very difficult given Ham's structure. There are definitely ways to reduce the scope of my previous attempt to remove Ham's String class, but Ham's pieces are tightly coupled and keeping Ham working reliably during a refactor would be a tremendous effort. Transferring Ham's logic to a new project structure allows for smaller changes that can be tested more reliably.

This refactor is mostly about cutting code, not adding it. The logic of Ham (how code is evaluated) can stay largely unchanged, other than some changes to the AST and data structures. The primary differences will be in delegating the parse, make, and test phases to external tools.

Ham's new goals can be found in [ADR #9 - Ham project goals](0009-ham-project-goals.md)

The parser and test framework are two pieces for which there are plenty of stable, user-friendly tools to choose from. For the build system backend, Ninja is essentially the only option, but it is widely used. The decisions behind each specific refactor are detailed in [ADR #10 - Using standard library containers](0010-use-standard-library-containers.md), [ADR #11 - Using PEGTL for parsing](0011-using-pegtl-for-parsing.md), [ADR #12 - Using Catch for testing](0012-using-catch-for-testing.md), and [ADR #13 - Using the Ninja build system](0013-using-the-ninja-build-system.md).

## Consequences
Doing one large refactor allows Ham to:
- Change many design decisions
- Clarify its own goals
- Reach stability before the end of the GSoC period
- Have a small, maintainable codebase going forward

Doing a large refactor like this has risk. It's common for large structural changes to have unforeseen blockers that can prevent completion. For this to be successful, there needs to be a strict focus on getting Ham working and stable rather than letting feature creep step in.
