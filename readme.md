# PRos32 / [![LICENSE MIT](https://img.shields.io/badge/LICENSE-MIT-blue)](https://opensource.org/license/mit) / ![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/pros-dev-fans/PRos32/build.yml) / ![Version](https://img.shields.io/badge/version-0.1.0--dev-orange?logo=git)

32-bit operating system written in GAS/C for x86, inspired by [x16-PRos](https://github.com/PRoX2011/x16-PRos) and made
by its community.

> [!IMPORTANT]
> TODO:
> - add screenshots, GIFs

## Features

> [!WARNING]
> This section is not complete yet.

## Building

### Method 1 _recommended_

> [!IMPORTANT]
> This method uses docker to provide you all the dependencies.
> You need to [install it](https://docs.docker.com/engine/install/) before using this method.

> [!WARNING]
> Do not run this script as root. It is not supported.

```bash
./build.sh
```

### Method 2

> [!IMPORTANT]
> This method assumes that you have already installed all the dependencies.

Here are the dependencies for Debian:

- `build-essential`
- `mtools`
- `dosfstools`
- `parted`
- `multipath-tools`
- `grub2-common`
- `grub-pc-bin`
- `e2fsprogs`
- `syslinux`

```bash
make
```

With both methods, you will get two images:

- one of them `fd.img`, which is _a kernel floppy disk image_
- second is `pros32-v.{PROS32-VERSION}.img`,  _a hard disk bootable image_

## Running

To run PRos32, use an x86 hypervisors/emulators such as **QEMU/VMware/Virtualbox**.

Here is an example of a command to run in QEMU

```bash
qemu-system-i386 -drive file=pros32-v.{PROS32-VERSION}.img,format=raw,if=ide,index=0
```

## Contributing

> [!WARNING]
> This section is not complete yet.

> [!IMPORTANT]
> TODO:
> - code of conduct

## License

This project is licensed under the [MIT License](https://opensource.org/license/mit/).

## Huge thanks

PRos32 exists thanks to x16-PRos; we are extremely thankful
to [![User](https://img.shields.io/badge/GitHub-PRoX2011-blue?logo=github)](https://github.com/PRoX2011) for creating
x16-PRos.
