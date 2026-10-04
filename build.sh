#!/bin/sh

delete() {
    echo "[BUILD SCRIPT] Delete build directory"
    rm -rf build
}

build=1
nobuild() {
    echo "[BUILD SCRIPT] Running without build"
    build=
}

run=1
norun() {
    echo "[BUILD SCRIPT] Building without run"
    run=
}

gdb=
gdb() {
    echo "[BUILD SCRIPT] Running with GDB"
    gdb=1
}

hdd=
hdd() {
    echo "[BUILD SCRIPT] Building HDD image"
    hdd=1
}

help() {
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

os=$(uname -s)
case "$os" in
    Linux*)
        GRUB2_BOOT_IMG="/usr/lib/grub/i386-pc/boot.img"
        GRUB2_MKRESCUE="/usr/bin/grub-mkrescue"
        GRUB2_MKIMAGE="/usr/bin/grub-mkimage"
        ;;
    FreeBSD*|OpenBSD*|NetBSD*|DragonFly*)
        GRUB2_BOOT_IMG="/usr/local/lib/grub/i386-pc/boot.img"
        GRUB2_MKRESCUE="/usr/local/bin/grub-mkrescue"
        GRUB2_MKIMAGE="/usr/local/bin/grub-mkimage"
        ;;
    *)
        echo "[BUILD SCRIPT] Unknown OS: $os – using default GRUB paths (may fail)"
        GRUB2_BOOT_IMG="/usr/lib/grub/i386-pc/boot.img"
        GRUB2_MKRESCUE="/usr/bin/grub-mkrescue"
        GRUB2_MKIMAGE="/usr/bin/grub-mkimage"
        ;;
esac

if [ ! -f "$GRUB2_BOOT_IMG" ]; then
    echo "[BUILD SCRIPT] Warning: $GRUB2_BOOT_IMG not found"
fi
if ! command -v "$GRUB2_MKRESCUE" >/dev/null 2>&1; then
    echo "[BUILD SCRIPT] Warning: $GRUB2_MKRESCUE not found in PATH"
fi
if ! command -v "$GRUB2_MKIMAGE" >/dev/null 2>&1; then
    echo "[BUILD SCRIPT] Warning: $GRUB2_MKIMAGE not found in PATH"
fi

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

if [ -n "$build" ]; then
    echo "[BUILD SCRIPT] Building PRosBSD"
    cmake -DCMAKE_TOOLCHAIN_FILE=toolchain/llvm-i686-pc-none-elf.cmake \
          -DGRUB2_BOOT_IMG="$GRUB2_BOOT_IMG" \
          -DGRUB2_MKRESCUE="$GRUB2_MKRESCUE" \
          -DGRUB2_MKIMAGE="$GRUB2_MKIMAGE" \
          -S . -B build
    if [ -n "$hdd" ]; then
        cmake --build build --target grub2_hdd_image
    else
        cmake --build build --target grub2_iso_image
    fi
    if [ "$?" -ne 0 ]; then
        exit 1
    fi
fi

if [ -n "$run" ]; then
    echo "[BUILD SCRIPT] Running PRosBSD"
    if [ -n "$gdb" ]; then
        if [ -n "$hdd" ]; then
            qemu-system-i386 -drive file=build/prosbsd-hdd.img,format=raw,if=ide,index=0 \
            -d int -no-shutdown -no-reboot -monitor stdio -s -S
        else
            qemu-system-i386 -cdrom build/prosbsd.iso \
            -d int -no-shutdown -no-reboot -monitor stdio -s -S
        fi
    else
        if [ -n "$hdd" ]; then
            qemu-system-i386 -drive file=build/prosbsd-hdd.img,format=raw,if=ide,index=0
        else
            qemu-system-i386 -cdrom build/prosbsd.iso
        fi
    fi
fi
