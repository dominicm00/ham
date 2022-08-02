# 12. Using the Ninja build system
Date: 2022-08-01

## Status
Accepted

## Context
Ham can either do the "make" phase (processing the dependency tree, executing commands) itself, or delegate it to Ninja. Currently, Ham's make phase is technically complete (it executes commands), but is far from optimal. It implements no caching, meaning incremental rebuilds are much slower than necessary.

### Benefits of Ninja
#### Stability
Ninja is a popular, well-tested tool, and will almost certainly have better stability (especially cross-platform) than a custom solution.

#### Debugging
Ninja supports advanced debugging output, including querying target dependencies and browsing the dependency graph visually. This supports Ham's debugging goal.

#### Clangd support
Ninja has native support for outputting a `clangd` compilation database. This supports Ham's editor support goal.

#### Speed
Ninja is focused on speed and would likely be faster than a custom solution, especially with its extensive caching and incremental build support. This supports Ham's speed goal.

### Downsides of Ninja
#### Runtime dependency
Using Ninja means Ham will have a runtime binary dependency instead of being standalone.

#### Loss of information
Outsourcing the make phase means Ham loses information about what targets are out of date and what commands are run. Ham would have to query Ninja for this information.

#### Loss of control
Outsourcing the make phase means Ham cannot precisely control how commands are run. It can only do operations supported by Ninja's interface. However, since Ninja is purpose-built to be used as an "assembler" by other build tools, it purposefully provides the user a lot of control.

## Decision
Ham will use the Ninja build system.

Matching Ninja's level of caching and incremental build speed is not feasible. Ninja will inevitably be faster and more stable than a custom solution for incremental builds (where speed is most important). If Ham delegates the entire make phase *and debugging output* to Ninja, it shouldn't need to access make information directly. Ninja's debugging output is likely more advanced than what Ham would be able to do by itself.

## Consequences
Using Ninja simplifies the make phase, cross-platform compatibility, and debugging.

It also introduces a runtime dependency and imposes constraints on the data Ham can reliably access.
