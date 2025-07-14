#!/bin/sh
docker build --build-arg HOST_UID=$(id -u) --build-arg HOST_GID=$(id -g) -t pros32-builder .
sudo docker run --privileged --rm -v "$(pwd):/pros32" -w /pros32 pros32-builder
