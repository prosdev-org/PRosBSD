#!/bin/bash



function delete {
    echo "[BUILD SCRIPT] Delete build directory"
    rm -rf build
}


build=1
function nobuild {
    echo "[BUILD SCRIPT] Running without build"
    build=
}


run=1
function norun {
    echo "[BUILD SCRIPT] Building without run"
    run=
}


sudo="sudo"
function nosudo {
    echo "[BUILD SCRIPT] Building without sudo"
    sudo=
}


function help {
    printf \
"Usage: ./build.sh [ARGUMENT ...]\n"\
"Arguments:\n"\
"    -d, --delete       - Delete build directory\n"\
"    -D, --delandexit   - Delete build directory and exit\n"\
"    -B, --nobuild      - Run without build\n"\
"    -R, --norun        - Build without run\n"\
"    -S, --nosudo       - Build without sudo (useful with docker user group)\n"\
"    -h, --help         - Print this page\n"\
"Example:\n"\
"    ./build.sh         - Build and run\n"\
"    ./build.sh -R      - Only build\n"\
"    ./build.sh -d -R   - Delete and build without run\n"
}


while [ -n "$1" ]; do
    case "$1" in
        -d | --delete) delete ;;
        -D | --delandexit) delete
                           exit ;;
        -B | --nobuild) nobuild ;;
        -R | --norun) norun ;;
        -S | --nosudo) nosudo ;;
        -h | --help) help
                     exit ;;
        *) echo "[BUILD SCRIPT] Unknown argument: $1"
           help
           exit ;;
    esac
    shift
done





if [[ $build ]]; then
    echo "[BUILD SCRIPT] Building PRosBSD"
    docker build --build-arg HOST_UID=$(id -u) --build-arg HOST_GID=$(id -g) -t prosbsd-builder .
    $sudo docker run --privileged --rm -v "$(pwd):/prosbsd" -w /prosbsd prosbsd-builder
fi

if [[ $run ]]; then
    echo "[BUILD SCRIPT] Running PRosBSD"
    qemu-system-i386 -drive file=build/prosbsd.img,format=raw,if=ide,index=0
fi
