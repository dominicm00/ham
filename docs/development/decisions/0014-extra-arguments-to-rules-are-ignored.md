# 1. Extra arguments to rules are ignored
Date: 2022-08-03

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
Jam ignores extra arguments to rules. Ignoring extra arguments also has utility since rule names can be determined dynamically.

## Decision
Ham will ignore extra arguments to rules. However, there should be a warning if too many arguments are passed to a *literal* rule name, as that is likely a mistake.

## Consequences
Compatibility with Jam in this area will be maintained. Dynamic rules will also be more straightforward.

Some otherwise easily found issues might slip by. This could be addressed with a debugging option.
