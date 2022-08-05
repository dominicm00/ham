#!/bin/sh
find src cmd include -type f -regex ".*\.\(cpp\|hpp\)$" | xargs clang-format -i
