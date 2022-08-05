# Forbid high power sources in piecemeal actions
Date: 2022-07-20

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Status](#status)
- [Context](#context)

<!-- markdown-toc end -->

## Status
Accepted

## Context
High power sources occur when the source variable, directly or indirectly, occurs multiple times in a single word:
```
# 2 = a b ;
# $(2)-$(2) => a-a a-b b-a b-b
```

High power sources have non-obvious behavior with piecemeal. Say you have the following action:
```
# 2 = a b c ;
actions piecemeal maxline 20 EchoSources
{
    echo $(2)-$(2)
}
```

You would probably expect the result to look like this:
```
# Each echo command only has space for 4 pairs at a time
a-a a-b a-c b-a
b-b b-c c-a c-b
c-c
```

However, it would actually look like this:
```
a-a a-b b-a b-b
c
```

Per the letter of the law, this is correct. The source set `{a, b, c}` was too large for the command buffer, so it was split into two separate commands running on the source sets `{a, b}` and `{c}`. However, this means losing source pairs there would have been otherwise.

There's no robust way to give the "obvious" behavior either. Say the action was this instead:
```
# 2 = a b c ;
actions piecemeal maxline 20 EchoSources
{
    echo $(2)-$(2) $(2)
}
```

There's no way to split the word elements while maintaining the relationship between the two words. Splitting by the elements in the expanded word only works if there is exactly one word using the source variable.

Currently, there is no known use case for high power sources in actions.
