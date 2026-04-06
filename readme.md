# PRosBSD / ![3-Clause BSD License](https://img.shields.io/badge/LICENSE-3--Clause_BSD-blue) / ![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/prosdev-org/PRosBSD/build.yml)

PRosBSD is an experimental OS written in C and C++, inspired by x16-PRos,
aiming for compatibility with the FreeBSD ABI and memory safety.

> [!IMPORTANT]
> TODO:
> - add screenshots, GIFs

## Rewrite notice

> [!IMPORTANT]
> This project is not dead. Check out the `rewrite/0.2.0` branch.

We decided to rewrite PRosBSD from scratch, as the current version (0.1.0-dev) in the `dev` branch is memory-unsafe crap full of UB's.
There is no point in continuing to support it further.
The `rewrite/0.2.0` branch will be merged into the `dev` branch as soon as these features are implemented:

- [x] VGA Text
- [x] print
- [x] panic
- [ ] Debugging macros (e.g., `DEBUG_*`)
- [x] Debug over GDB
- [ ] GDT
- [ ] IDT
- [ ] Normal heap
- [ ] Memory map
- [ ] Paging
- [ ] CMOS
- [ ] PS/2 Keyboard
- [ ] PIC Timer
- [x] Grub
- [ ] KShell
- [ ] PATA PIO driver
- [ ] Some filesystem

## Building

> [!IMPORTANT]
> This method uses docker to provide you all the dependencies.
> You need to [install it](https://docs.docker.com/engine/install/) before using this method.

> [!WARNING]
> Do not run this script as root. It is not supported.

```bash
./build.sh -R
```

## Running

To run PRosBSD, use an x86 hypervisors/emulators such as **QEMU/VMware/Virtualbox**.

Here is an example of a command to run in QEMU

```bash
./build.sh -B
```

## Contributing

> [!WARNING]
> This section is not complete yet.

> [!IMPORTANT]
> TODO:
> - code of conduct

## License

This project is licensed under the [3-Clause BSD License](https://opensource.org/license/bsd-3-clause).

## Huge thanks

PRosBSD exists thanks to x16-PRos; we are extremely thankful
to [![User](https://img.shields.io/badge/GitHub-PRoX2011-blue?logo=github)](https://github.com/PRoX2011) for creating
x16-PRos.
