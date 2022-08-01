# 10. Use standard library containers
Date: 2022-08-01

## Status
Accepted

## Context
Ham currently uses its own containers modeled from Haiku. These containers do not have have consistent interfaces around iterators/memory management, and reference counted objects need to be incremented/decremented manually, leading to double frees/memory leaks.

Not using standard library containers also hampers interoperability with libraries and consuming code.

The standard library has some issues, such as a lack of helper functions, and poorly made modules like the regex module.

## Decision
Ham will use standard library containers, instead of its own.

## Consequences
The interfaces for containers will no longer be the same as Haiku, but they should be familiar to anyone who has coded modern C++ elsewhere.

Proper use of the standard library should lead to less memory issues, better type checking, and more reliable implementations.

Code can be simplified to use algorithms in the standard library instead of manual ones.
