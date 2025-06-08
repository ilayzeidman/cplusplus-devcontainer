FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gdb meson ninja-build pkg-config \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-tools gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly gstreamer1.0-libav \
    libgtest-dev  libgmock-dev cmake libboost-all-dev gstreamer1.0-x \
    libglib2.0-dev x11-apps \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

USER vscode
WORKDIR /workspaces/${LOCAL_WORKSPACE_FOLDER_NAME}
