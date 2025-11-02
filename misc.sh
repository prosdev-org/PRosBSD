#!/bin/bash



function clang_format {
    echo "[MISC SCRIPT] Reformatting code"
    find . \( -name '*.h' -o -name '*.c' \) -print0 | xargs -0 clang-format -i
}


function help {
    printf \
"Usage: ./build.sh [ARGUMENT ...]\n"\
"Arguments:\n"\
"    -F, --clang-format    - Apply clang formatting\n"
}


while [ -n "$1" ]; do
    case "$1" in
        -F | --clang-format) clang_format ;;
        *) echo "[MISC SCRIPT] Unknown argument: $1"
           help
           exit ;;
    esac
    shift
done
