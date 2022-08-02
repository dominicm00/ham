# 11. Using Catch for testing
Date: 2022-08-01

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Status](#status)
- [Context](#context)
  - [Tools](#tools)
- [Decision](#decision)
  - [Section based](#section-based)
  - [Data generators](#data-generators)
- [Consequences](#consequences)

<!-- markdown-toc end -->

## Status
Accepted

## Context
Ham's testing needs are fairly simple; fundamentally, it just takes in data, processes it, and spits out output. Any test framework would likely work.

### Tools
Tools considered as a testing framework were:
- [Doctest](https://github.com/doctest/doctest)
- [Catch](https://github.com/catchorg/Catch2)
- [GoogleTest](https://github.com/google/googletest)
- [Boost.Test](https://www.boost.org/doc/libs/1_79_0/libs/test/doc/html/index.html)

## Decision
Ham will use Catch as it's testing framework.

### Section based
Catch uses sections instead of fixtures for setup, which is (in my opinion) more natural.

### Data generators
Catch makes running the same assertions on multiple pieces of data a simple one-liner.

## Consequences
Using Catch instead of a custom framework provides better integration with build systems/CI, and a more complete feature set.

Using Catch means certain types of tests, like mocking and death tests, are more difficult.
