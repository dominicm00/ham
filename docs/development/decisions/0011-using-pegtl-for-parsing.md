# 9. Using PEGTL as the parsing solution

Date: 2022-08-01

## Status

Accepted

## Context

### Parsing architectures
Parsing architectures can be broken down into a few components.

#### DSL vs library
DSL tools like Bison and ANTLR have their own DSL that is compiled down to C++ code. These tools require a binary dependency at build time, and require learning special syntax. However, they can be more concise than tools confined to C++ syntax.

Libraries are just C++ code, and generally use template metaprogramming to approximate their own DSL. These do not require binary dependencies and are simpler to build/integrate, but may be more verbose.

#### CFG vs PEG
Context free grammars (specifically LR-style parsers or ones that emulate them) are able to express grammars with few restrictions. The most notable difference is that they can accept left-recursive grammars, where the first symbol is non-terminal. This is common in binary expressions:
```
exp + exp
exp - exp
exp * exp
exp / exp
```

The above expression cannot be represented naturally in LL/PEG (i.e. recursive descent) parsers, because it would recurse endlessly down the left side trying to match an expression.

The downside of LR parsers is that they are more complex, and have difficult to resolve errors (shift-reduce conflicts) that require specifying rule associativity and precedence. While associativity and precedence are straightforward for arithmetic, there are often more complex cases. The textbook example is `if (x) then if (y) then a else b`. Associativity determines whether this is parsed as `if (x) then (if (y) then a) else b` or `if (x) then (if (y) then a else b)`.

LL parsers function similarly to recursive descent parsers. They are simpler, but can't express left-recursive constructs.

PEG parsers essentially formalize recursive descent parsers. Namely, there is no associativity or precedence; every `or` statement is ordered. This generally makes PEG tools simpler than their CFG counterparts.

### Tools
Below are tools that were considered for Ham's parser.

#### Flex/Bison
A replacement for Lex/Yacc, Flex/Bison is a universally used toolchain for parser-generators. Bison generates a LALR parsing with a CFG grammar, meaning care has to be taken to prevent ambiguous parsing and precedence issues. However, it can express powerful grammars concisely.

#### ANTLR
A more modern (but still very mature) parser-generator, ANTLR is an LL(*) parser with a CFG grammar. However, it has tooling to transform non-left-recursive grammars into LL grammars, meaning it's still very expressive. Like Bison, it uses a DSL and requires a build-time binary dependency.

#### Boost.Spirit
An expression template library that approximates the syntax of external parser-generators in C++ code. Boost.Spirit is infamous for long compile times (e.g. 20+ minutes for real-world grammars), but this may have changed in the 3.0.0 release.

#### PEGTL
The most mature and active (to my knowledge) PEG grammar library for C++, PEGTL is a header-only library that builds parsers via template metaprogramming and inheritance. It has many additional features outside the core parsing library, including automatically building parse trees and providing debugging output.

#### Lexy
A relatively new library, Lexy is a PEG grammar tool with an operator based DSL. It aims to be fast and stick close to the format of a handwritten recursive descent parser.

## Decision
Ham will use PEGTL as its parsing tool.

### Simplicity
PEGTL is a relatively simple tool, both in its construction and use, which supports Ham's simplicity goal.

### C++ library
Having a parser as code makes it easier to test, and has a shorter learning curve than a dedicated tool. Ham does not need a language-independent grammar, so it doesn't suffer downsides from being locked into C++.

### PEG parsing
Jam has a simple grammar designed for recursive descent tools, and doesn't have left-recursive constructs that often occur in programming languages. PEG grammars are able to express Jam in a simple, performant way.

### Parsing rules as units
Since a PEGTL grammar is just a top level rule, any rule can be trivially unit tested. This makes parser development easier than if only the entire grammar could be tested. It's also easy to make slightly different versions of a grammar as you can reuse rules.

### Separate actions and grammar
Unlike traditional tools, PEGTL separates semantic actions and grammars. This means you can, at runtime, select different actions to run on the same grammar.

### Mature and tested
PEGTL's v1.0 came out 7 years ago (2015), and had 6 years of 0.x releases before then. It is still actively maintained, thoroughly tested, and has good documentation.

### Debugging
PEGTL can output a trace of a parsing run, or output an `xdot` visualization of a parse tree. This makes parser development much easier.

### Exceptions
PEGTL has good support for parsing exceptions. Mainly, you can decide whether a local parsing failure stays local (try the next thing), or becomes global (fail immediately). This lets you limit errors to specific constructs, instead of mysterious `expected <symbol>` error messages.

### AST support
PEGTL has built in support for creating an AST.

## Consequences
PEGTL is relatively simple to use, and should be easy for developers to work with. 

As a PEG grammar, it's more verbose to express nested left recursive operators if Ham decides to add them.
