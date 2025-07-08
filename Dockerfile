FROM debian:latest

RUN apt update && apt install -y \
    build-essential \
    mtools dosfstools \
    parted \
    multipath-tools \
    grub2-common \
    grub-pc-bin \
    wget \
    e2fsprogs \
    nasm \
    sudo \
    && rm -rf /var/lib/apt/lists/*

RUN wget -O /opt/memdisk https://github.com/redox-os/isolinux/raw/refs/heads/master/memdisk

ARG HOST_UID=0
ARG HOST_GID=0

RUN groupadd -g $HOST_GID builder \
    && useradd -u $HOST_UID -g $HOST_GID -m -s /bin/bash builder \
    && echo 'builder ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER builder

CMD ["make"]
