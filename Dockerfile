FROM debian:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    mtools \
    dosfstools \
    parted \
    multipath-tools \
    grub2-common \
    grub-pc-bin \
    e2fsprogs \
    syslinux \
    sudo \
    curl \
    git \
    && rm -rf /var/lib/apt/lists/*

ARG HOST_UID=0
ARG HOST_GID=0

RUN groupadd -g $HOST_GID builder \
    && useradd -u $HOST_UID -g $HOST_GID -m -s /bin/bash builder \
    && echo 'builder ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER builder

RUN NONINTERACTIVE=1 \
    bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
ENV PATH="/home/linuxbrew/.linuxbrew/bin:/home/linuxbrew/.linuxbrew/sbin:${PATH}"

RUN brew install i686-elf-gcc

CMD ["sh", "-c", "cmake -DCMAKE_TOOLCHAIN_FILE=toolchain-i686-elf.cmake -S . -B build; cmake --build build"]
