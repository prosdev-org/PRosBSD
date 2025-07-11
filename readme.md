# PRos32 / [<img src="https://img.shields.io/badge/LICENSE-MIT-blue" alt="LICENSE MIT">](https://opensource.org/license/mit)

32-bit operating system written in NASM/C for x86, inspired by [x16-PRos](https://github.com/PRoX2011/x16-PRos) and made
by its community.

> [!IMPORTANT]
> TODO:
> - add screenshots, GIFs

## Features

> [!WARNING]
> This section is not completed yet.

## Building

### Method #1 _recommended_

> [!IMPORTANT]
> This method involves docker to deliver you all the dependencies.
> You have to [install it](https://docs.docker.com/engine/install/) before using this method.

```bash
./build.sh
```

### Method #2

> [!IMPORTANT]
> This method assumes that you have all the dependencies installed.

Here is Debian's dependencies's packages:

- `build-essential`
- `mtools`
- `dosfstools`
- `parted`
- `multipath-tools`
- `grub2-common`
- `grub-pc-bin`
- `e2fsprogs`
- `nasm`
- `syslinux`

```bash
make
```

In both methods, you will get two images:

- one of them `fd.img` _kernel floppy disk image_
- second is `pros32.img` _hard disk multiboot image_

## Running

To run PRos32, use x86 hypervisors/emulators such as **QEMU/VMware/Virtualbox**.

Here is an example command for running in QEMU

```bash
qemu-system-i386 -drive file=pros32.img,format=raw,if=ide,index=0
```

## Contributing

> [!WARNING]
> This section is not completed yet.

> [!IMPORTANT]
> TODO:
> - code of conduct

## License

This project is licensed under the [MIT License](https://opensource.org/license/mit/).

## Huge thanks

PRos32 exists thanks to x16-PRos; we are extremely thankful
to [![User](https://img.shields.io/badge/GitHub-PRoX2011-blue?logo=github)](https://github.com/PRoX2011) for creating
x16-PRos.
