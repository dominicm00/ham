# 6. Semantics of together action modifier

Date: 2022-07-14

## Status

Accepted

## Context

Jam does not clearly specify how `together` action modifiers work when there are multiple targets. There are many possible interpretations of `together` with multiple targets; see the discussion in [#50](https://github.com/dominicm00/ham/pull/50) for more details. 

## Decision

Ham will throw an error if more than one target is passed to a `together` action.

## Consequences

This is technically incompatible with Jam, but Jam does not seem to have any reasonable behavior in this case so it's unlikely user code relies on it. Other desirable behavior is relatively easy to replicate with rules, so this shouldn't lock users out of anything.
