#!/bin/sh
find . \
    \( -path ./build -o -path ./cmake-build-debug -o -path ./.git \) -prune \
    -o \
    \( -name '*.c' -o -name '*.h' -o -name '*.cxx' -o -name '*.hxx' \) \
    -exec clang-format -i {} +
