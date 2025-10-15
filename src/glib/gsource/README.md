# FD-driven GSource Example

This example demonstrates how to create a custom GSource that monitors file descriptors (FDs) using GLib's event loop system. This is particularly useful for handling sockets, pipes, and other file descriptors in a non-blocking manner.

## Overview

The example implements a **FD-driven GSource** that:
- Monitors a pipe for readable data
- Uses the classic `g_source_add_poll()` API with `GPollFD`
- Demonstrates proper GSource lifecycle management
- Provides graceful shutdown handling

## Key Concepts

### GSource Functions

The custom source implements four key functions:

1. **`pipe_prepare()`** - Called before polling to set timeout behavior
2. **`pipe_check()`** - Called after polling to check if source is ready
3. **`pipe_dispatch()`** - Called when source is ready to handle the event
4. **`pipe_finalize()`** - Called when source is being destroyed

### FD Monitoring APIs

GLib provides two APIs for FD monitoring:

- **Modern (UNIX)**: `g_source_add_unix_fd()` / `g_source_query_unix_fd()`
- **Classic**: `g_source_add_poll()` with `GPollFD` (used in this example)

## Files

- `custom_source_fd.c` - Main implementation with complete working example
- `Makefile` - Build configuration with helper script generation
- `README.md` - This documentation

## Building

### Prerequisites

Ensure you have GLib development libraries installed:

```bash
# Ubuntu/Debian
sudo apt-get install libglib2.0-dev

# Check if dependencies are available
make check-deps
```

### Compilation

```bash
# Build everything
make

# Or build individual components
make custom_source_fd
make send_to_pipe.sh
```

### Available Make Targets

- `make all` - Build executable and helper script
- `make clean` - Remove all built files  
- `make run` - Build and run the program
- `make test` - Show testing instructions
- `make help` - Display help information
- `make check-deps` - Verify dependencies

## Running the Example

### Method 1: Interactive Mode

1. **Start the program:**
   ```bash
   make run
   # or
   ./custom_source_fd
   ```

2. **The program will display:**
   ```
   FD-driven GSource Example
   ========================
   
   Created pipe: read_fd=3, write_fd=4
   Pipe source created and attached (ID: 1)
   
   To test the pipe, run in another terminal:
     echo 'hello' > /proc/12345/fd/4
     echo 'world' > /proc/12345/fd/4
     echo 'quit' > /proc/12345/fd/4
   ```

3. **In another terminal, send data:**
   ```bash
   # Replace 12345 with actual process ID and 4 with actual write_fd
   echo 'hello' > /proc/12345/fd/4
   echo 'test message' > /proc/12345/fd/4
   echo 'quit' > /proc/12345/fd/4  # This will stop the program
   ```

### Method 2: Using Helper Script

```bash
# Run the main program
./custom_source_fd &

# Use the generated helper script (in another terminal)
./send_to_pipe.sh <write_fd>
```

## Code Structure

### Data Structure

```c
typedef struct {
  GSource source;    // Base GSource (must be first!)
  GPollFD pfd;      // File descriptor + events + revents
} PipeSource;
```

### Source Creation

```c
static GSource* pipe_source_new(int fd) {
  PipeSource *ps = (PipeSource*) g_source_new(&pipe_funcs, sizeof *ps);
  ps->pfd.fd = fd;
  ps->pfd.events = G_IO_IN;        // Monitor for readable data
  ps->pfd.revents = 0;
  g_source_add_poll((GSource*)ps, &ps->pfd);
  return (GSource*) ps;
}
```

### Event Handling

The `pipe_dispatch()` function:
- Reads available data from the pipe
- Processes special commands (like "quit")
- Handles errors and EOF conditions
- Returns `TRUE` to continue monitoring, `FALSE` to remove source

## Features Demonstrated

1. **Custom GSource Implementation** - Complete lifecycle management
2. **File Descriptor Monitoring** - Non-blocking FD operations
3. **Graceful Shutdown** - Signal handling and clean resource management
4. **Error Handling** - Proper handling of read errors and EOF
5. **Interactive Testing** - Multiple ways to test the implementation

## Usage Patterns

This pattern is useful for:

- **Network Programming** - Monitoring sockets for incoming connections/data
- **IPC (Inter-Process Communication)** - Pipes, FIFOs, Unix domain sockets
- **File Monitoring** - Though `GFileMonitor` is usually preferred
- **Device Files** - Serial ports, special devices
- **Custom Protocols** - Any FD-based communication

## Signal Handling

The example includes proper signal handling:

- **SIGINT (Ctrl+C)** and **SIGTERM** trigger graceful shutdown
- Main loop is stopped cleanly
- Resources are properly released

## Cleanup

The example demonstrates proper cleanup:

1. Destroy and unref the GSource
2. Unref the main loop
3. Close file descriptors
4. Exit gracefully

## Extending the Example

To adapt this for other use cases:

1. **Change the FD type** - Replace pipe with socket, device file, etc.
2. **Modify events** - Use `G_IO_OUT` for write readiness, `G_IO_ERR` for errors
3. **Add multiple FDs** - Create multiple sources or use `g_source_add_unix_fd()` multiple times
4. **Custom data processing** - Modify `pipe_dispatch()` for your protocol/format

## Related Examples

- `custom_source_timeout.c` - Timer-based GSource example
- GLib documentation on [GSources](https://docs.gtk.org/glib/struct.Source.html)
- [GLib Main Event Loop](https://docs.gtk.org/glib/main-loop.html) documentation

## Troubleshooting

### Common Issues

1. **"Permission denied" when writing to `/proc/PID/fd/FD`**
   - Ensure you're using the correct process ID and file descriptor number
   - The process must still be running

2. **"No such file or directory"**
   - The file descriptor may have been closed
   - Check that the process is still running with `ps aux | grep custom_source_fd`

3. **Helper script not working**
   - Ensure the main program is running first
   - Check that `pgrep -f custom_source_fd` finds the process

### Debugging

Add debug prints or use GDB:

```bash
# Compile with debug symbols (already enabled in Makefile)
make clean && make

# Run with GDB
gdb ./custom_source_fd
(gdb) run
# In another terminal, send data to trigger events
```

## License

This example is provided for educational purposes and can be freely used and modified.