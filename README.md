# cplusplus-devcontainer

# gstmypassthrough

A **minimal “do-nothing” GStreamer plugin** written in modern C++ (C++17) and built with Meson + Ninja.  
The element simply lets every buffer flow unchanged from its sink pad to its source pad, making it the perfect skeleton on which to base your own custom filters.

> **Why another pass-through?**  
> Because the smallest possible, fully functional example is often the best learning tool.  
> This repo shows every moving part—GObject boilerplate, Meson build files, and a ready-to-use VS Code **devcontainer** that lets you debug the element under GDB with breakpoints set directly in the IDE.

---

## Contents

| Path | Purpose |
|------|---------|
| `mypassthrough.cpp` | The element implementation (`GstBaseTransform` subclass). |
| `meson.build` | Top-level Meson project file. |
| `.vscode/` | Tasks and debug launchers for local *or* devcontainer use. |
| `.devcontainer/` | Dockerfile and configuration for VS Code Remote – Containers. |

---

## Quick Start (Local Build)

```bash
# 0. Prerequisites (Ubuntu/Debian):
sudo apt update && sudo apt install -y \
  build-essential meson ninja-build pkg-config \
  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

# 1. Configure once
meson setup build                 # creates ./build
# 2. Compile whenever you change code
meson compile -C build            # or: ninja -C build
# 3. Tell GStreamer where the plugin lives
export GST_PLUGIN_PATH=$PWD/build
gst-inspect-1.0 mypassthrough     # should list the element
# 4. Try a pipeline
gst-launch-1.0 -v videotestsrc ! mypassthrough ! autovideosink
