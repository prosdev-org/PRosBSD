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
    && rm -rf /var/lib/apt/lists/*

ARG HOST_UID=0
ARG HOST_GID=0

RUN groupadd -g $HOST_GID builder \
    && useradd -u $HOST_UID -g $HOST_GID -m -s /bin/bash builder \
    && echo 'builder ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER builder

CMD ["sh", "-c", "cmake -S . -B build; cmake --build build"]
