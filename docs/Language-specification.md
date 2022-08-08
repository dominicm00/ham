# Ham Language Specification

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Comments](#comments)
- [Statements](#statements)
- [Identifiers](#identifiers)
- [Leafs](#leafs)
  - [Words](#words)
  - [Double quoted strings](#double-quoted-strings)
  - [Single quoted strings](#single-quoted-strings)
  - [String combinations](#string-combinations)
  - [Bracket expressions](#bracket-expressions)
  - [Leaf expansion](#leaf-expansion)
  - [Observations](#observations)
- [Lists](#lists)
- [Variables](#variables)
  - [Variable expressions](#variable-expressions)
    - [Variable identifier](#variable-identifier)
    - [Variable subscripts](#variable-subscripts)
    - [Variable modifiers](#variable-modifiers)
  - [Variable assignment](#variable-assignment)
- [Rules](#rules)
  - [Defining rules](#defining-rules)
  - [Invoking rules](#invoking-rules)
  - [Built-in rules](#built-in-rules)
- [Actions](#actions)
  - [Defining actions](#defining-actions)
  - [Invoking actions](#invoking-actions)
  - [Action modifiers](#action-modifiers)
- [Control structures](#control-structures)
  - [Conditions](#conditions)
  - [If statement](#if-statement)
  - [While loop](#while-loop)
  - [For loop](#for-loop)
  - [Break and continue](#break-and-continue)
- [Target statements](#target-statements)
- [Dependencies](#dependencies)
  - [Direct dependencies](#direct-dependencies)
  - [Sibling dependencies](#sibling-dependencies)
  - [Order dependencies](#order-dependencies)
  - [Header dependencies](#header-dependencies)
  - [Pseudotargets](#pseudotargets)

<!-- markdown-toc end -->

## Comments
Comments start with `#` and continue until the end of the line.

## Statements
Statements are a series of [rule invocations](#invoking-rules), [action invocations](#invoking-actions), [variable assignments](#variable-assignment), [control structures](#control-structures), or [target statements](#target-statements).

## Identifiers
Identifiers are the names of rules, actions, and variables. Ham uses the same identifier scheme as C. The first character must be a non-digit of the form `a-z A-Z _`, and the remaining characters must be of the form `a-z A-Z _ 0-9`.

Ham identifiers are case sensitive.

## Leafs
Leafs are the core data structure in Ham, and consist of everything that can be evaluated to a string list (even a string list of size 0 or 1). A leaf literal is comprised of one or more string forms, [variables](#variables), or [bracket expressions](#bracket-expressions) not separated by whitespace. There are three string forms:

### Words
Words are character sequences of all printable, non-whitespace characters (ASCII 33-126), _excluding the following:_ `" ' #`.

The `$` character indicates the start of a variable expression. Not having a valid variable expression after a `$` in a word is an error.

**Examples:**
```text
word
!with#@symbols%^&(
<grist>my/path/file.cpp
C:\\%appdata%
400.0
$(myvar)

$400.0    # NOT a valid word
"hello"   # NOT a valid word, but is a valid string
hi'again' # NOT a valid word, but is a valid string
```

### Double quoted strings
Double quoted strings are character sequences of escape sequences, printable characters (ASCII 32-126), tab, newline, carriage return, vertical tab, and form feed, surrounded by non-escaped double quotes.

Double quoted strings recognize the following escape sequences:

| Sequence | ASCII byte | Meaning         |
|:---------|:-----------|-----------------|
| `\a`     | `0x07`     | audible bell    |
| `\b`     | `0x08`     | backspace       |
| `\f`     | `0x0c`     | form feed       |
| `\n`     | `0x0a`     | new line        |
| `\r`     | `0x0d`     | carriage return |
| `\t`     | `0x09`     | horizontal tab  |
| `\v`     | `0x0b`     | vertical tab    |
| `\'`     | `0x27`     | single quote    |
| `\"`     | `0x22`     | double quote    |
| `\\`     | `0x5c`     | backslash       |
| `\$`     | `0x24`     | dollar sign     |

A `\` followed by an invalid escape sequence is an error. Use `\\` for literal backslashes.

The `\'` escape sequence is permitted, but optional.

The `$` character indicates the start of a variable expression. Not having a valid variable expression after a `$` in a double quoted string is an error. The escape sequence `\$` can be used to insert `$` as a literal.

Note that literal whitespace is allowed, and will be translated as-is.

The string will be terminated at the first non-escaped double quote. Not having an end quote is an error.

**Examples:**
```text
"quoted string"
"\a with \t escape \n sequences \t"
"a $(valid) variable"
"\$400.0"
"with 'single quotes' nested"
"escaped \" quote"

"with
literal whitespace"

"$400.0"   # NOT a valid double quoted string
```

### Single quoted strings
Single quoted strings are character sequences of printable characters (ASCII 32-126), tab, newline, carriage return, vertical tab, and form feed, surrounded by non-escaped single quotes.

Single quoted strings *do not recognize escape sequences or variable expansions*.

Note that literal whitespace is allowed, and will be translated as-is.

The string will be terminated at the first single quote. Not having an end quote is an error.

**Examples:**
```text
'my "Weird string" with $symbols'

'with
literal whitespace'

# Escape sequences are not recognized so the below counts as having an unclosed quote.
'\''    # NOT a valid single quoted string.
```

### String combinations
Adjacent string forms or variables are treated as a single leaf and concatenated.

**Examples:**
```text
path/with/'Weird $Directory'/file.cpp    => path/with/Weird $Directory/file.cp
"combining "'forms 'in-any" which way"   => combining forms in-any which way
```

### Bracket expressions
Bracket expressions have the following form (literal `[]`, not optional):
```text
[ <statement> ]
```

A bracket expression evaluates to the result of the `statement`. They may only be used with statements that have a value (i.e. no control structures).

**Examples:**
```text
rule Return1 {
  return 1 ;
}

Echo [ Return1 ] ; # 1

X on target = 2 ;
Echo [ on target $(X) ] ; # 2
```

### Leaf expansion
Leafs with [variable expressions](#variable-expressions) evaluate to the product of their component parts. For every set of variable elements, variable expressions are evaluated and the resulting leaf added to the result.

With one variable, you can think of expansion as a map operation:
```text
X = a b c ;
Echo t$(X) ;   # ta tb tc
Echo $(X)t ;   # at bt ct
Echo *$(X)* ;  # *a* *b* *c*
```

With more than one variable, the result is slightly more complex:
```text
X = a b c ;
Y = 1 2 ;
W = "" "" ;
Z = ;
Echo $(X)-$(X) ;   # a-a a-b a-c b-a b-b b-c c-a c-b c-c
Echo $(X)-$(Y) ;   # a-1 a-2 b-1 b-2 c-1 c-2
Echo $(X)-$(W) ;   # a- a- b- b- c- c-
Echo $(X)-$(Z) ;   # <empty list>
```

The process can be thought of as a series of nested for loops, with the leftmost variable expressions taking the outermost loops. The rightmost variable expressions are iterated, and when complete the next variable to the left increments.

### Observations
- Words can accept almost anything without whitespace.
- The combo of words and single quotes is great for describing paths.
```text
$(myroot)/path/'Big Directory'/file
'C:\\single\quotes\accept\backslashes'
```
- Double quotes are similar to C strings.
- Only double quotes can describe a `'` character.
- Expansions are exponential based on the number of variable expressions; be careful.

## Targets
A target is a string consisting of (in order):
- An optional grist surrounded by `<>` brackets
- A relative or absolute filepath valid _on the running system_

Built-in rules that act on targets compare equality with the target's string representation.

**NOTE:** Ham is considering methods for better cross-platform file paths, but it is currently the user's responsibility. Using paths invalid on the running system is **undefined behavior**, and may or may not work with or without a warning.

**Examples:**
```
relative/path.cpp

# Absolute path for Unix
/my/absolute/path.cpp

# Absolute path for Windows
C:\\another\absolute\path.cpp

# These two are different targets to Ham
nodir.cpp
<grist>nodir.cpp
```

### Target binding
Before being executed by commands, targets generally have to be bound to a filesystem path. The binding process differs between sources and generated targets:

#### Source target binding
[Source targets](#source-dependencies) are rooted at each directory in the target-local `$(SEARCH)` variable, and the first one where the file exists is set as the target path. If the target represents an absolute path, `$(SEARCH)` is ignored. If no existing file is found, an error is thrown.

#### Generated target binding
Generated targets are rooted at the target-local `$(LOCATE)` variable. If the target represents an absolute path, `$(LOCATE)` is ignored.

**Examples:**
```text
# src/main.cpp already exists

rule EchoCompile {
  Depends $(1) : $(2) ;
}

actions EchoCompile {
  echo Compiled $(1) with $(2) ;
}

SEARCH on main.cpp = extern src ;
LOCATE on mybin = bin ;

# main.cpp found in src/
# mybin rooted at bin/
EchoCompile mybin : main.cpp ; # Compiled bin/mybin with src/main.cpp
Depends all : mybin ;
```

## Lists
Lists contain 0 or more leafs. A list literal is a whitespace delimited list of leafs.

**Examples:**
```text
this is "a list" # [this, is, a list]
```

## Variables
Variables names are identifiers. Variables hold lists. Variables are either global, local to the block scope, or local to a target.

### Variable expressions
Data inside variables must be accessed via variable expressions. Evaluating a variable expression always results in a list, even if the list contains 0 or 1 element. Variables are evaluated with the following form: 
```text
$( <identifier|variable>[subscript][modifiers] )
```

#### Variable identifier
Evaluating an identifier returns the associated string list:
```text
X = 123 ;
Echo $(X) ; # 123
```

Alternatively, the identifier can be determined dynamically with another variable expression:
```text
X = 123 ;
Y = X ;
Echo $(Y) ; # X
Echo $($(Y)) ; # 123
```

This can be nested arbitrarily.

#### Variable subscripts
A subscript is of the form `[<leaf>]` (literal `[]`, not optional), where the leaf evaluates to one of the following:
- `<integer>`
- `<integer>-`
- `<integer>-<integer>`


These correspond, respectively, to selecting via index from the variable's list:
- The single element at the index
- The range, starting from the first index, to the end of the list
- The _closed range_ from the first index to the second index

List indexes start at `1` and go to the length of the list.

**Examples:**
```text
X = a b c d ;
Y = 1-3 ;
Z = 2 ;
W = 3 ;

Echo $(X[1]) ;          # a
Echo $(X[2-]) ;         # b c d
Echo $(X[2-3]) ;        # b c
Echo $(X[$(Y)]) ;       # a b c
Echo $(X[$(Z)]) ;       # b
Echo $(X[$(Z)-]) ;      # b c d
Echo $(X[$(Z)-$(W)]) ;  # b c
```

#### Variable modifiers
Modifiers consist of `:`, a character, and optionally `=` followed by an argument.

The following modifiers are available:

| Modifier       | Result                                      |
|:---------------|:--------------------------------------------|
| `:B`           | Select base filename                        |
| `:S`           | Select last filename suffix                 |
| `:D`           | Select directory path                       |
| `:G`           | Select grist                                |
| `:U`           | Uppercase contents                          |
| `:L`           | Lowercase contents                          |
| `:B=base`      | Replace base                                |
| `:S=suffix`    | Replace suffix                              |
| `:D=directory` | Replace directory path                      |
| `:G=grist`     | Replace grist                               |
| `:R=root`      | Root file if not an absolute path           |
| `:E=value`     | Default value if variable is unset or empty |
| `:J=separator` | Concatenate list elements with a separator  |

Ham will attempt to "do what you mean" with path modifiers and insert/remove separators where appropriate. For instance:
```text
X = dir/file.cpp

# Both of these are dir/file.hpp
$(X:S=hpp)
$(X:S=.hpp)

# This removes the suffix
$(X:S=) # dir/file

# Both of these are newdir/file.cpp
$(X:D=newdir)
$(X:D=newdir/)

# Multiple separators in the replacer are respected
$(X:D=newdir//) # newdir//file.cpp
```

**TODO:** This part of the spec is uncertain. Paths are just strings in Jam, and are not treated semantically, so it's not clear how to offer a better cross-platform path mechanism while remaining backwards compatible (specifically, how to change separators by the platform). For now, the recommendation is just to avoid specifying separators where possible until Ham has a mechanism for this.

Changes from each modifier are "fed" into the next modifier. Modifiers have the following precedence in order of evaluation, with lower precedence evaluated first. Modifiers of the same precedence are independent and can be considered to be evaluated simultaneously.

| Modifier       | Precedence                       |
|:---------------|:---------------------------------|
| `:E=value`     | 1                                |
| `:B=base`      | 2                                |
| `:S=suffix`    | 2                                |
| `:D=directory` | 2                                |
| `:G=grist`     | 2                                |
| `:R=root`      | 2                                |
| `:B`           | 3                                |
| `:S`           | 3                                |
| `:D`           | 3                                |
| `:G`           | 3                                |
| `:U`           | 4 (mutually exclusive with `:U`) |
| `:L`           | 4 (mutually exclusive with `:L`) |
| `:J=separator` | 5                                |

If a modifier follows another modifier without an argument, the colon may be omitted. For instance, `:BS` selects the filename (base + suffix). Colons must be included to separate modifiers with arguments, such as `:R=/:D=usr`. Notably, this means words used as a modifier argument cannot contain `:`; you should use double/single quoted strings instead.

**Examples:**
```text
X = /absolute/path.cpp relative/path.cpp ;
Echo $(X:B=file) ;   # /absolute/file.cpp relative/file.cpp
Echo $(X:S=.hpp) ;   # /absolute/path.hpp relative/path.hpp
Echo $(X:R=/usr) ;   # /absolute/path.cpp /usr/relative/path.cpp

Y = a/long/path/file.cpp ;
Echo $(Y:D=home) ;   # home/file.cpp
Echo $(Y:G=grist) ;  # <grist>a/long/path/file.cpp
W = <grist>path/to/file.cpp ;
Echo $(W:B) ;        # file
Echo $(W:S) ;        # .cpp
Echo $(W:D) ;        # path/to
Echo $(W:G) ;        # <grist>

Echo $(Z:E=val) ;    # val

A = path/file.cpp ;
Echo $(A:BSB=head) ; # head.cpp
Echo $(A:UD=root) ;  # ROOT/FILE.CPP

B = file1.cpp file2.cpp file3.cpp ;
Echo $(B:J=", ") ; # file1.cpp, file2.cpp, file3.cpp
Echo $(B:BJ=", ") ; # file1, file2, file3
Echo $(B:UJ=x) ;    # FILE1xFILE2xFILE3
```

### Variable assignment
Variables are assigned to with the following form:
```text
[local] <identifier> [on <targets>] <operator> <list> ';'
```

The assignment operators are:

- `=`: set the variable to the list
- `+=`: append the list to the variables data, and set the variable to the result
- `?=`: set the variable to the list if and only if the variable is unset or contains the empty list

The optional `on <targets>` form sets the target-local variable instead of the global variable.

The optional `local` form only sets the variable within the current scope.

The `local` and `on <targets>` form are mutually exclusive; including them both in an assignment is an error.

**Examples:**
```text
X = 123 ;
Y on t1 t2 = 456 ;

Echo $(X) ; # 123
Echo $(Y) ; # error: evaluated unset variable
[ on t1 Echo $(X) $(Y) ] ; # 123 456

A ?= val1 ; # A is initially unset, so ?= sets A
A = ;
A ?= val2 ; # A is the empty list, so ?= sets A
Echo $(A) ; # val2

B = 123 456 ;
B += 789 ;
Echo $(B) ; # 123 456 789
```

## Rules
Rules are Ham's version of functions, and the core of its functionality.

### Defining rules
Rules are defined with the following form:
```text
rule <identifier> [<identifier> [: <identifier>]*] { 
    <statements>
}
```

Arguments are bound as `local` variables with the names specified in the rule header. Arguments are also represented positionally with built-in variables:
- `$(<)`: the first argument
- `$(>)`: the second argument
- `$(1), $(2),...$(9)`: arguments 1-9 respectively

Global and local variables are bound within the rule statement, but local variables are bound from the **calling scope**, not from the definition scope.

**TODO:** Taking local variables from the call site is a necessary evil for compatibility. This should be a compatibility behavior.

Rule definitions have access to the special statement `return <list> ;`. When this statement is evaluated, the rule immediately stops and the value is the result of the rule evaluation. Passing more than one argument to `return` is an error. A single argument that is a list is valid.

```text
return 1 2 3 ;      # ok
return 1 : 2 : 3 ;  # error
```

### Invoking rules
Rules are invoked with the form:
```text
<identifier> [<list> [: <list>]*] ;
```

Any arguments not passed to the rule default to the empty list. Additional arguments past what the rule accepts are ignored.

**Examples:**
```text
rule Echo2 x : y {
    Echo $(x) $(y) ;
}

Echo2 a b : 1 2 ; # a b 1 2
Echo2 a b ;       # a b
Echo2 ;           #
Echo2 a : b : c ; # a b
```

### Built-in rules
Ham has several built-in rules to access core functionality. For compatibility reasons, these rules can be accessed with UPPERCASE, lowercase, or CamelCase casing.

| Rule                                    | Description                                                                                                            |
|:----------------------------------------|:-----------------------------------------------------------------------------------------------------------------------|
| `Include <file> ;`                      | Includes a Jamfile in the current statement; acts as a "paste" (no scoping)                                            |
| `Depends <targets> : <sources> ;`       | Creates [direct dependencies](#direct-dependencies) from every `target` to every `source`                              |
| `Includes <targets> : <sources> ;`      | Creates [sibling dependencies](#sibling-dependencies)  from every `target` to every `source`                           |
| `MaybeIncludes <targets> : <sources> ;` | Creates [ordered sibling dependencies](#order-dependencies) from every `target` to every `source`                      |
| `MaybeDepends <targets : <sources> ;`   | Creates [ordered direct dependencies](#order-dependencies) from every `target` to every `source`                       |
| `NotFile <targets> ;`                   | Marks `targets` as [pseudotargets](#pseudotargets) instead of real files                                               |
| `Always <pseudotargets> ;`              | Builds specified `pseudotargets` _if they are in the dependency tree_, regardless of their dependency status           |
| `Echo <message> ;`                      | Prints `"$(message:J=" ")\n"` to the standard output                                                                   |
| `Exit <message> ;`                      | Prints `"$(message:J=" ")\n"` to the standard output, and exits with a failure status                                  |
| `Glob <directories> : <patterns> ;`     | Search `directories` with glob `patterns` and return a list of matching files                                          |
| `Match <regexps> : <strings> ;`         | Matches regex expressions against `strings`, concatenating matches within each string and returning the resulting list |

There are several rules which Ham recognizes as built in rules, but do not currently have any defined functionality.

**TODO:** The behavior of these rules is not defined until the semantics that translate best to Ninja are decided on.

| Rule                    | Jam behavior                                                  | Notes                                                                                |
|:------------------------|:--------------------------------------------------------------|--------------------------------------------------------------------------------------|
| `LEAVES <targets> ;`    | Makes `targets` only dependent on their leaf sources.         | Can't think of a use case.                                                           |
| `NOCARE <targets> ;`    | Ignore targets that do not exist and cannot be made.          | Probably don't need this with header scanning; maybe replace with `EXISTS` predicate |
| `NOUPDATE <targets> ;`  | Ignore timestamps on targets.                                 | This is essentially for directories, and Ninja (should?) handle that                 |
| `TEMPORARY <targets> ;` | Allow targets to be deleted after their dependents are built. |                                                                                      |


## Actions
Actions are commands run in the system's shell. All actions implicitly take two arguments.

Generally, `$(1)` represents targets to be built, and `$(2)` represents sources the target is built from. Ham will allow you to run actions where this is not the case, with the following caveats:

- If the action does not build the files specified by `$(1)`, it will be re-run on each Ham invocation, as the targets are missing.
- Running actions where `$(1)` does not depend on `$(2)` should be considered compatibility behavior, and is a bad idea. If you want to pass information to a command that is not a source, you should use target-local variables.
- Invoking an action on `$(1)` multiple times is allowed if and only if the same `together` action is used.

Any non-leaf target **must** have an action to build it.

### Defining actions
Actions are defined with the following form:
```text
actions <modifiers> <identifier> {
    <string>
}
```

`shell-input` is treated as a literal strings to be passed to the system's command shell, with the exception of the following special sequences:
- `$(<variable-expression>)`: Targets, sources, and variables specified by the `bind` modifier are [bound](#target-binding). The whitespace-delimited portion containing the variable expression is then evaluated as a leaf.
- `\$`: Escapes the `$` character and prevents it from being interpreted as a variable expression.
- `\}`: Escapes the `}` character and prevents it from being interpreted as the end of the shell input.

### Invoking actions
Actions are invoked just like rules, and can in fact have the same name as a rule. If an action and rule have the same name, the rule is evaluated first, and then the action is invoked.

**Examples:**
```text
rule EchoVar target {
    X on $(target) = 1 2 3 ;
}

actions EchoVar {
    echo $(X)
}

X on t = 4 5 6 ;
Depends all : t ;

EchoVar t ; # outputs 1 2 3
```

### Action modifiers
Action modifiers are a list of keywords in the action definition. Modifiers sometimes accept an argument in `[]` brackets. The following modifiers are accepted:

| Modifier                   | Description                                                                                                     |
|:---------------------------|:----------------------------------------------------------------------------------------------------------------|
| `existing`                 | Limit `$(2)` to files that already exist.                                                                       |
| `updated`                  | Limit `$(2)` to files that need to be updated.                                                                  |
| `ignore`                   | Treat the command as successful regardless of the return status.                                                |
| `together`                 | The action can be invoked on a target multiple times, concatenating the sources.                                |
| `bind[<vars>]/bind <vars>` | Bind variables to filesystem paths. For legacy reasons, has a second form that must be the last modifier.       |
| `deps[make : <file>]`      | Specifies that the command outputs additional [make dependencies](#header-dependencies) to `file`.              |
| `deps[msvc]`               | Specifies that the command outputs additional [msvc dependencies](#header-dependencies) to the standard output. |

**Examples:**
```text
actions existing Rm {
  rm $(2)
}

# when 'rm' is built, only existing sources are removed
Rm rm : source1 source2... ;
```

## Control structures
### Conditions
Let `a` and `b` be leaf expressions, and `cond` represent another condition. A condition takes one of the following forms:

| Form             | Condition to be true                                               |
|:-----------------|:-------------------------------------------------------------------|
| `a`              | any element of `a` is a non-empty string                           |
| `a = b`          | all elements of `a` and `b` are equal                              |
| `a != b`         | `a = b` is false                                                   |
| `a < b `         | `a[i] < b[i]`, where `i` is the first element where `a[i] != b[i]` |
| `a > b`          | `a[i] > b[i]`, where `i` is the first element where `a[i] != b[i]` |
| `a <= b`         | `a[i] <= b[i]` holds for all `i < min(len(a), len(b))`             |
| `a >= b`         | `a[i] >= b[i]` holds for all `i < min(len(a), len(b))`             |
| `a in b`         | `a` is a subset of `b`, or `a` is empty                            |
| `! cond`         | `cond` is false                                                    |
| `cond && cond`   | both conditions are true                                           |
| `cond || cond`   | either condition is true                                           |
| `(cond)`         | precedence grouping                                                |

### If statement
An if statement is of the form:
```text
if <condition> { <statements> } [ else { <statements> } ]
```

If the condition is true, the first statement block is evaluated. Otherwise, the else block is evaluated (if present).

### While loop
A while loop is of the form:
```text
while <condition> { <statements> }
```

Repeatedly executes `statements` while `condition` is true upon entry.

### For loop
A for loop is of the form:
```text
for <identifier> in <leaf> { <statements> }
```

For each element in `leaf`, `statements` are executed with `identifier` bound to the element.

### Break and continue
The `break ;` and `continue ;` statements may be used inside loops to exit the loop and continue to the next loop iteration respectively. Using `break ;` or `continue ;` outside a loop is illegal.

## Target statements
A statement may be run under the influence of target specific variables with the following form:
```text
on <target> <statement>
```

The `return` statement may be used within a target statement to return a value.

**Examples:**
```text
X on target = 1 2 3 ;

on target for var in $(X) { Echo $(var) ; } # 1\n 2\n 3\n
on target Echo $(X) ; # 1 2 3
Y = [ on target return $(X[2]) ] ;
Echo $(Y) ; # 2
```

## Dependencies
A dependency is an asymmetrical relationship with a parent and child. 

A dependency relationship enforces that:

- The child is built before the parent
- If the child is updated, the parent is rebuilt

### Direct dependencies
The `Depends` rule creates a direct dependency between a parent and child:
```text
Depends foo.o : foo.c ;
```

### Sibling dependencies
The `Includes` rule creates a sibling dependency, where anything that includes the child also includes the siblings. This is useful if you need to explicitly specify a header dependency.
```text
Includes foo.c : generated.h ; 
Depends foo.o : foo.c ; # generated.h needs to be built before we try and build foo.o
```

Notice that this is **not the same** as just using `Depends`. If we did the following:
```text
Includes foo.c : generated.h ; 
Depends foo.o : foo.c ; # bang! can't build foo.c
```

`foo.c` would no longer be a leaf source, and Ham would not know how to build it.

### Order dependencies
The `MaybeIncludes` and `MaybeDepends` rules create order-only dependencies, where:

- The child is built before the parent, but
- Updating the child does not update the parent

This is useful if you don't know ahead of time if a source file includes a generated header. Using `MaybeIncludes` will generate the header on the first run, but only rebuild the parent if it actually ended up using the header.

### Source dependencies
Source dependencies are targets that don't have any associated actions, and cannot be built (e.g. source files). All source dependencies must be [bound](#target-binding) to an existing file, and cannot have any dependencies.

### Header dependencies
Ham provides facilities to determine C/C++ header dependencies automatically. Using this requires compiler support, which all major Unix compilers and `msvc` provide.

Dependencies should be determined as a side-effect of compilation. This means you don't have perfect knowledge of dependencies before compilation. Normally this is fine, as the header dependency is only used to rebuild the file on header changes. However, generated header files cannot be automatically included this way on the first run; use `MaybeIncludes` to fix this.

If the tool supports outputting dependencies in `make` format to a file (`gcc`, `clang`, etc.), use the `deps[make : <file>]` action modifier. For instance:
```text
rule deps[make : $(1).d] Cc {
  gcc -MD -MF $(1).d $(2) -o $(1)
}
```

Note that the deps files are considered temporaries and removed automatically.

`msvc` supports a different dependency format, and prints to standard output instead of a file. To enable `msvc` dependencies, first define your action as before:
```text
rule deps[msvc] Cc {
  cl /showIncludes -c $in /Fo$out
}
```

Then globally define the prefix `msvc` prints before each dependency:
```text
MSVC_DEPS_PREFIX = "Note: including file:" ;
```

Unfortunately, this prefix is locale specific; users will have to manually set this for their system.

### Pseudotargets
Pseudotargets are declared with the `NOTFILE` rule, and represent targets that are not actual files.

Pseudotargets without actions act as aliases for their dependencies:
```text
NOTFILE pseudo ;
Depends pseudo : header1 header2 ;
Includes foo.c : pseudo ; # foo.c has a sibling relationship with header1 and header2
```

Pseudotargets with actions act as scripts, and are always run since the pseudotarget does not exist.
```text
actions Whoami {
  whoami
}

NOTFILE whoami ;
Whoami whoami ; # the 'Whoami' action is run whenever 'ham whoami' is run
```
