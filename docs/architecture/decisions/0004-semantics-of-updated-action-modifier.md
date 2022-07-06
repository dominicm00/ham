# 4. Semantics of `updated` action modifier

Date: 2022-07-06

## Status

Accepted

## Context

Jam's language documentation does not have a detailed specification describing what an updated target is. Replicating Jam's behavior is not possible since the `updated` action modifier is inconsistent when the source has dependents other than the action target.

Ham requires a stricter definition of an updated target, both for end-users and for development.

## Decision

An updated target is one that is newer than the **action target**. In the action invocation `Action <target> : <sources>`, the time is compared against `<target>`, _not_ one or more of `<sources>` dependents. A source that is being made is always updated by definition. Having multiple targets in an `updated` action is an error.

## Consequences

This definition ensures that a target always "sees" when a source is updated. For instance, say `oldTarget` and `newTarget` both use a source in an updated action. If a source has a single definition of being updated based on it's dependencies, either `oldTarget` would not get the source, or `newTarget` would despite it not being updated relative to it. This is both unreliable, and breaks the principle that parents/siblings should not be able to affect a target's build.

Because the time is compared against the action target, this means the behavior with multiple actions is unclear. Ham chooses to avoid confusion by throwing an error, but there are other options such as comparing against the newest/oldest target. It's possible that other features, such as parameters to action modifiers, could enable different behavior in the future.
