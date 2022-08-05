# 9. Ham project goals
Date: 2022-08-01

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Status](#status)
- [Context](#context)
  - [Original goals](#original-goals)
    - [No user facing changes](#no-user-facing-changes)
    - [Jam compatibility](#jam-compatibility)
  - [Missing goals](#missing-goals)
- [Decision](#decision)
  - [Backwards compatibility](#backwards-compatibility)
  - [Debugging](#debugging)
  - [Documentation](#documentation)
  - [Editor support](#editor-support)
  - [Rulesets](#rulesets)
  - [Language improvements](#language-improvements)
  - [Speed](#speed)
  - [Stability](#stability)
  - [Simplicity](#simplicity)
  - [Platform compatibility](#platform-compatibility)
- [Consequences](#consequences)

<!-- markdown-toc end -->

## Status
Accepted

## Context
Ham is being refactored, as described in [ADR #8](0008-creating-a-version-2.md). Ham should have defined project goals in order to evaluate architectural/feature choices.

### Original goals
Ham's original goals were:

- Provide a modern C++ replacement to Jam
- Be bug-for-bug compatible with Jam (and possibly Boost.Build)
- Prioritize speed

Some problems arose with these goals.

#### No user facing changes
Ham had no concrete goals for user facing improvements other than speed (and, related, caching). Speed is definitely important, but there's also opportunities to make Jam itself more accessible. Because Jam's syntax is so simplistic, it is relatively easy to add new language extensions.

#### Jam compatibility
First, bug-for-bug compatibility with Jam is impossible in some situations (see [#50](https://github.com/dominicm00/ham/pull/50#issuecomment-1170568688)). Ham could only replicate simple and reliable bugs, but it's questionable if that is actually helpful for the user. For example, when presented with a range selection with an end subscript before the end of the list (say, `[2-3]` on a list of length 5), the empty list would be returned. Ham will replicate this behavior, but that's likely not what a user expects.

The original reasoning behind this compatibility was so that Ham could be dropped into any existing Jam build system without any changes. However, if Ham doesn't offer significant improvements over Jam, there is no motivation to switch to it at all. Plus, there will inevitably be small implementation differences between Ham and Jam.

While complete backwards compatibility is not feasible, necessitating large changes in build systems is unreasonable (and also not the point of Ham), so Ham should still maintain a large degree of backwards compatibility.

### Missing goals
Ham's original goals left several important features out of scope.

Jam's language specification ranges from vague (action modifiers) to outright incomplete (no mention of `[ ]` syntax). Jam tutorials are also rather lacking. Documentation to supplement/replace Jam's would be useful to users.

Most of the time people spend working on build systems is debugging them. Debugging support should be a top priority for a build system.

Modern editor support, especially for a C/C++ build system, is a must to get IDE features.

## Decision
Ham will create a list of goals that apply to Ham as a general tool, not just a Haiku project. The only prerequisite for Ham to be ready for Haiku is building Haiku successfully on all platforms. These goals describe additional constraints for when Ham is ready outside of Haiku.

Following is the new list of goals for Ham. Each goal is broken down into sub-goals with the following keyword format:

- **will**: Top priority goals. These should be completed before the 1.0.0 release.
- **will probably**: Secondary goals. Important nice-to-haves that are not critical for Ham's immediate use.
- **may**: Tertiary goals. It would be ok to focus on these if there are no other important tasks.
- **will probably not**: Semi-non-goals that are likely out of scope for Ham, but may later be changed.
- **will not**: Non-goals that will not be implemented in Ham.

### Backwards compatibility
Ham **will** remain backwards compatible with idiomatic Jam build systems. This means Jam build systems that don't rely on bugs or otherwise unspecified behavior should work correctly. This does not necessarily mean that Ham will exactly follow the specification, as there are some aspects that need to be changed for performance or aren't clearly defined.

Ham **will** document any notable behavioral changes between Ham and Jam.

Ham **will** provide a clear transition path for any modified/missing behavior.

Ham **will probably** offer compatibility options for behavioral changes that are not bugs, if deemed necessary/feasible.

Ham **will not** try to replicate Jam bugs. It will instead focus on helping users find bugs, offering facilities to see places where there might be behavioral changes. Users who cannot afford to have changes to their build system should use Jam, as it's not feasible to guarantee identical behavior across implementations.

### Debugging
Ham **will** provide comprehensive debugging output. The minimum bar is spewing out all of Ham's evaluation information, but of course more user-friendly tools are preferred. Ham's debugging support should be able to provide info for specific constructs, whether than be a target or rule. 

Ham **will** produce errors/warnings on unexpected input. Issues will not be silently fixed in the background.

Ham **will probably** have source level errors/warnings. Being able to see exactly what caused the error stops simple errors quickly.

Ham **will probably not** include an interactive debugger. This is possible (Boost.Build does it), but improvements to debugging output should cover most use cases for something like Ham.

### Documentation
Ham **will** develop, and maintain, a complete language specification. Behavior undefined in the specification should be considered bugs.

Ham **will** maintain a complete reference of command line options.

Ham **will** maintain a complete reference of built-in rules for all integrated rulesets.

Ham **will** document architectural decisions with ADRs (like this one).

Ham **will** document its overall architecture.

Ham **will** document any public-facing library functions.

Ham **will** document, and update, these goals as necessary.

Ham **will** document its contribution process.

Ham **will probably** document key internal functions.

Ham **will probably** develop tutorials. This should be done gradually due to time constraints.

### Editor support
Ham **will** work with the [`clangd`](https://clangd.llvm.org/) language server. The minimum here is doing nothing, as `bear` can intercept compilation calls and make a compilation database. Ham would just have to test that `bear` works as expected. However, it's greatly preferred to export a compilation database directly, so you don't have to do a clean build of the project to get editing features.

Ham **may** provide plugins for common editors, including syntax highlighting, generating compilation databases automatically, or target selection/building directly in the editor. The scope is purposely broad so that Ham can slowly add features where possible.

### Rulesets
Ham **will**, at a minimum, build-in and maintain the Perforce Jambase and Haiku Jambase. This includes modifying them if there are syntactical/behavioral changes.

Ham **will probably not** include the Boost.Build build system. The `b2` interpreter has diverged from `jam`, and including it has questionable benefits (after all, `b2` is already a usable tool). This may change after discussions with the Boost.Build team.

Ham **will probably not** develop its own ruleset. Ham is primarily used for projects with large, existing Jam build systems, not for new projects. Considering the availability modern tools, Ham is unlikely to see much use in newer projects.

### Language improvements
New language features **will** be opt-in. Preferably the syntax of new language features should be optional, so that the original syntax and new syntax can be used at the same time. If that is not possible, the feature must be behind a flag. The default language must remain compatible with Jam.

Ham **may** make format changes to the Jam language, like not requiring a space before semicolons. All such changes should be optional, and accompanied by tooling to move to the new format.

### Speed
Ham **will**, at a minimum, be faster than Jam. Cases where Ham is slower should be considered bugs.

Ham **will probably** provide automated benchmarks to guard against performance regressions.

### Stability
Ham **will** prioritize fixing bugs over new features.

Ham **will** have a comprehensive test suite. Test coverage must be run regularly, and should be as high as reasonably possible. There must be tests for, at a minimum:

- All public (i.e. resulting in an AST node) parsing rules
- The evaluation of all AST nodes
- All non-node language features (e.x. action modifiers)
- All public library functions
- Correct building of Haiku

Ham **will** keep the main branch deployable. Tests will be run in CI, and only pull requests passing CI may be merged. Commits may not be pushed directly outside trivial changes (documentation, formatting, etc.).

Ham **will** test release candidates with end users before releasing a version officially. Once a release candidate is created, no new features can be added. The only permitted changes are for bugs found during the testing stage.

### Simplicity
Ham **will** prioritize simplicity in design. Simplicity should only be sacrificed for speed when benchmarks show a significant need. Marginal/theoretical improvements should not complicate the architecture.

### Platform compatibility
Ham **will** be compatible with all platforms Haiku can be built on.

Ham **will probably** be compatible with Windows. Windows compatibility is more complex than Unix platforms.

## Consequences
With a defined set of goals/non-goals Ham has a framework to evaluate architectural/development choices. The focus on debugging and user-facing improvements should give Ham the value proposition for projects to use it exclusively.

The change in backwards compatibility will cause some breakage, but should be minimal (Haiku's build system works almost as-is).
