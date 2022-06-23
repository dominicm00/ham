#!/bin/sh
find src -type f -regex ".*\.\(cpp\|hpp\)$" | xargs clang-format -i
