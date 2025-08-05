#!/bin/sh
docker build --build-arg HOST_UID=$(id -u) --build-arg HOST_GID=$(id -g) -t prosbsd-builder .
sudo docker run --privileged --rm -v "$(pwd):/prosbsd" -w /prosbsd prosbsd-builder
