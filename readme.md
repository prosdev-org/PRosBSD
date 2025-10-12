# PRosBSD / ![3-Clause BSD License](https://img.shields.io/badge/LICENSE-3--Clause_BSD-blue) / ![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/prosdev-org/PRosBSD/build.yml) / ![Version](https://img.shields.io/badge/version-0.1.0--dev-orange?logo=git)

32-bit operating system written in GAS/C for x86, inspired by [x16-PRos](https://github.com/PRoX2011/x16-PRos) and made
by its community.

> [!IMPORTANT]
> TODO:
> - add screenshots, GIFs

## Features

> [!WARNING]
> This section is not complete yet.

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
