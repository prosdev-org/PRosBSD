#!/bin/bash



function clang_format {
    echo "[MISC SCRIPT] Reformatting code"
    find . \( -name '*.h' -o -name '*.c' \) -print0 | xargs -0 clang-format -i
}


function version_badge {
    echo "[MISC SCRIPT] Updating version badge"
    sed -i "s|https://img.shields.io/badge/version-[^?)]*|https://img.shields.io/badge/version-$(< build/version sed 's/-/--/g')-orange|" readme.md
}


function help {
    printf \
"Usage: ./build.sh [ARGUMENT ...]\n"\
"Arguments:\n"\
"    -F, --clang-format    - Apply clang formatting\n"\
"    -B, --version-badge   - Update version badge\n"
}


while [ -n "$1" ]; do
    case "$1" in
        -F | --clang-format) clang_format ;;
        -B | --version-badge) version_badge ;;
        *) echo "[MISC SCRIPT] Unknown argument: $1"
           help
           exit ;;
    esac
    shift
done
