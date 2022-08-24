#!/bin/sh
find src cmd include tests -type f -regex ".*\.\(cpp\|hpp\)$" | xargs clang-format -i
