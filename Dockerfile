# Build environment for CI and local use: host compiler for unit tests,
# arm-none-eabi for the firmware. The repo is mounted at /project at run time.
FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        ninja-build \
        git \
        ca-certificates \
        gcc-arm-none-eabi \
        libnewlib-arm-none-eabi && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /project
