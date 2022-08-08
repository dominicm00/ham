# 15. Don't implement member archive functionality
Date: 2022-08-08

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Status](#status)
- [Context](#context)
- [Decision](#decision)
- [Consequences](#consequences)

<!-- markdown-toc end -->

## Status
Accepted

## Context
Jam has a feature to represent archive members as targets. This feature is mostly undocumented, doesn't have clear semantics, non-portably bypasses the system's archive tool, and is not compatible with Haiku.

## Decision
Ham will, at least for the time being, not implement this feature. It's not clear what it's supposed to do, and I cannot find an example.

## Consequences
This is incompatible with Jam, but Haiku cannot use this feature, and until I find an example illustrating its semantics it is difficult for me to consider an implementation.
