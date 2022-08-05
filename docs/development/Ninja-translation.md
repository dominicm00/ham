# Ninja translation

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Naive translation](#naive-translation)
- [Possible optimizations](#possible-optimizations)
  - [Deduplication](#deduplication)
  - [Require targets to depend on sources](#require-targets-to-depend-on-sources)

<!-- markdown-toc end -->

## Naive translation
This Ham code:
```text
actions Cc {
    gcc $(CC_FLAGS) $(2) -o $(1)
}

Cc target : source1 source2 ;
Depends target : source1 source3 ;
```

Would transform to:
```text
rule Cc
    command = gcc $CC_FLAGS $inputs -o $outputs

build target: gcc source1 source3
    inputs = source1 source2
    outputs = target
    $CC_FLAGS = ...
```

Notice that inputs/outputs are specified manually instead of using `$in/$out`; this is because Ham action inputs don't correspond to the dependency tree. Variables used in the action are also specified per-target since Ham action variables are local.

## Action modifiers
Some action modifiers require modifications to the generated Ninja rules.

### Existing
Outputs passed to command are determined dynamically with a small helper.
```text
rule Cc
    command = gcc $CC_FLAGS $$(existing $inputs) -o $outputs
```

Another option would be to stat all the sources before building the command, and only put existing ones. However this means the Ninja build file is not a reproducible product of the Jamfiles, which is not desirable for caching.

### Updated
Similar to existing, except that since we don't know at the time of building the Ninja files what will be updated, we _have_ to determine the inputs dynamically.
```text
rule Cc
    command = gcc $CC_FLAGS $$(updated $inputs -o $out) -o $outputs
```

### Ignore
Depending on the platform, modify the command so it always returns a successful error status.
```text
rule Cc
    command = gcc $CC_FLAGS $$(updated $inputs -o $out) -o $outputs || exit 0
```

### Deps
The `deps*` modifiers correspond directly to Ninja dependency rules:
```text
rule Cc
    depfile = <file>
    deps = <gcc|msvc>
    command = gcc $CC_FLAGS $$(updated $inputs -o $out) -o $outputs || exit 0
```

## Pseudotargets
Pseudotargets can be declared with the `phony` rule:
```text
build <pseudotarget>: phony <dependencies>
```

## Possible optimizations
### Deduplication
Most of the time, the target does actually depend on its sources. We could determine a better location for an input based on the combination of how it's used in the action/its place in the dependency tree.

|               | Dependency          | Not dependency |
|:-------------:|:-------------------:|:--------------:|
| In action     | Explicit dependency | Extra inputs   |
| Not in action | Implicit dependency | N/A            |

```text
rule <rulename>
    command = ... $in $extra_inputs -o $outputs

build <target>: <rulename> <explicit-dependencies> | <implicit-dependencies>
    extra_inputs = <extra-inputs>
    outputs = <target>
    $CC_FLAGS = ...
```

### Require targets to depend on sources
Note that it doesn't make sense for a built target to not depend on its sources. The reason Ham allows it at all is because targets are used for things that aren't being "built", like `clean` or `uninstall`. However, this generally makes everything else more error prone.

Requiring targets to depend on their sources would improve the vast majority of Ham code, and things like `clean` should be using `bind` with a variable. Using `Clean clean` is really just a hack around `existing` and `piecemeal` only working on `$(2)`. There's also no deduplication mechanism for variables.
