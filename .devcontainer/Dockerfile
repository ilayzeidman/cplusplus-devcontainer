# Image de base légère + C++ 17
FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Dépendances système
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gdb meson ninja-build pkg-config \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-tools gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good gstreamer1.0-libav \
    libgtest-dev  libgmock-dev cmake libboost-all-dev \
    libglib2.0-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Crée l’utilisateur non-root « vscode » (fourni par l’image de base)
USER vscode
WORKDIR /workspaces/${LOCAL_WORKSPACE_FOLDER_NAME}
