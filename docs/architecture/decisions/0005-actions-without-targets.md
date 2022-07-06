# 5. Actions without targets

Date: 2022-07-06

## Status

Accepted

## Context

Actions do special actions with their target list, such as using the variable scope of the first target, comparing against the target with `together` and `updated`, etc. An action without a target can't be invoked via dependencies. Currently there are no restrictions on the number of targets passed to an action.

## Decision

Passing zero targets to an action should be an error.

## Consequences

Helps catch bugs, as actions without targets are useless and error-prone.
