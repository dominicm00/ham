# Architecture
This document describes the high-level architecture of Ham, and provides a birds-eye view of the codebase.

## Codemap

### `src/behavior`
Compatibility-related code; describes the behavioral differences between different Jam interpreters so Ham can match
their behavior as closely as possible.

### `src/code`
Defines the Ham (Jam) language and how to evaluate it. Includes [nodes](@ref ham::code::Node) that define basic language constructs, language behavior like [variables](@ref ham::code::LocalVariableDeclaration) and [blocks](@ref ham::code::Block), and the [evaluation context](ham::code::EvaluationContext).

### `src/data`
Data-types used throughout the codebase. The primary types to take note of are [paths](@ref ham::data::Path) and [targets](@ref ham::data::Target).

**NOTE:** The `data/String` types are slated to be replaced with modern standard library types.

### `src/make`
Executes actions and builds targets. The [Processor](@ref ham::make::Processor) class is the main entry point to Ham
(from a library perspective) and contains the logic to scan header files, build the dependency tree, determine which
targets should be updated, and execute the appropriate actions.

### `src/parser`
Parses Jamfiles into a [Block](@ref ham::code::Block).

**NOTE:** This custom parser is slated to be replaced with bison/flex.

### `src/platform`
Defines platform-specific operations, namely spawning and waiting on processes.

### `src/process`
Defines a general interface for interacting with system processes.

### `src/test`
Ham's test framework.

**NOTE:** Much of the test framework is slated to be replaced by [Catch](https://github.com/catchorg/Catch2), although some Ham-specific functionality (like [data based tests](@ref ham::test::DataBasedTest)) will remain.

### `src/tests`
Unit tests and a small test runner.

**NOTE:** The tests will change significantly with the introduction of [Catch](https://github.com/catchorg/Catch2).

### `src/util`
General utilities used throughout the codebase

### `testdata`
Data based end-to-end tests.
