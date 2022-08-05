# 3. Use shell scripts for ruleset preprocessing
Date: 2022-06-21

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
Rulesets from various Jam-style tools need to be processed so they can be statically included in the ham binary. Jam did this with a C script.

## Decision
We will use a shell script (`build-ruleset.sh`) to process rulesets.

## Consequences
Shell scripts are easier to make, add changes to, and build than C scripts. However, shell scripts may be slightly less portable by requiring tools like `sed` and `tr`.
