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

gdb=
function gdb {
    echo "[BUILD SCRIPT] Running with GDB"
    gdb=1
}


hdd=
function hdd {
    echo "[BUILD SCRIPT] Building HDD image"
    hdd=1
}




function help {
    printf \
"Usage: ./build.sh [ARGUMENT ...]\n"\
"Arguments:\n"\
"    -d, --delete       - Delete build directory\n"\
"    -D, --delandexit   - Delete build directory and exit\n"\
"    -B, --nobuild      - Run without build\n"\
"    -g, --gdb          - Run QEMU with GDB support\n"\
"    -h, --hdd          - Build HDD image\n"\
"    --help             - Print this page\n"\
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
        -g) gdb ;;
        -h | --hdd) hdd ;;
        --help) help
                     exit ;;
        *) echo "[BUILD SCRIPT] Unknown argument: $1"
           help
           exit ;;
    esac
    shift
done





if [[ $build ]]; then
    echo "[BUILD SCRIPT] Building PRosBSD"
    cmake -DCMAKE_TOOLCHAIN_FILE=toolchain/llvm-i686-pc-none-elf.cmake -S . -B build
    if [[ $hdd ]]; then
        cmake --build build --target grub2_hdd_image
    else
        cmake --build build --target grub2_iso_image
    fi
    if [ "$?" -ne 0 ]; then
        exit 1
    fi
fi

if [[ $run ]]; then
    echo "[BUILD SCRIPT] Running PRosBSD"
    if [[ $gdb ]]; then
        if [[ $hdd ]]; then
            qemu-system-i386 -drive file=build/prosbsd-hdd.img,format=raw,if=ide,index=0 \
            -d int -no-shutdown -no-reboot -monitor stdio -s -S
        else
            qemu-system-i386 -cdrom build/prosbsd.iso \
            -d int -no-shutdown -no-reboot -monitor stdio -s -S
        fi
    else
        if [[ $hdd ]]; then
            qemu-system-i386 -drive file=build/prosbsd-hdd.img,format=raw,if=ide,index=0
        else
            qemu-system-i386 -cdrom build/prosbsd.iso
        fi
    fi
fi
