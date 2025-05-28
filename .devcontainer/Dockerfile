FROM ubuntu:24.04

# Install build tools and dependencies
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        git \
        wget \
        libgtest-dev \
        libgmock-dev \
        gdb

# Build GoogleTest and GoogleMock (required for Ubuntu 24.04)
#RUN cd /usr/src/gtest && cmake . && make && cp lib/*.a /usr/lib && \
#    cd /usr/src/gmock && cmake . && make && cp lib/*.a /usr/lib

# Clean up
RUN apt-get clean && rm -rf /var/lib/apt/lists/*
