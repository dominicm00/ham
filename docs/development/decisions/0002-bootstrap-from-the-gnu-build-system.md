# 2. Bootstrap from the GNU build system

Date: 2022-06-10

## Status

Accepted

## Context

Ham needs to have a clear bootstrapping mechanism. While the previous Bootstrap.sh script worked, it was single-threaded and not portable. Since the bootstrapping mechanism will be the primary build mechanism for packagers, users, and even developers before Ham is complete, it should be fast, portable, and follow existing conventions. It should also be easy to maintain as the codebase evolves.

## Decision

Autotools will be used to implement the GNU build system.

## Consequences

The ubiquity of the GNU build system means software using it can be built on nearly all Unix systems, and it's conventions are well understood. Distribution tarballs created by Autotools also don't require additional dependencies, easing the burden on users and packagers.

Ham plans to take a dependency on Yacc/Lex style tools, which Autotools has good support for, so users on exotic systems can use whatever Yacc/Lex compatible tools they have available.

Building from the source repository will require a dependency on Autotools, slightly increasing the complexity for users not using a release tarball.

Autotools can be difficult to use, but this is offset by the simplicity of Ham's build needs (a single binary and shared library).

The GNU build system doesn't have good support for Windows, but Ham itself does not explicitly intend to work on non-Unix systems.
