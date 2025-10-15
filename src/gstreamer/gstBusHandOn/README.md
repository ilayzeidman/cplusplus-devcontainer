# GSource-based GstBus Example

This example demonstrates how to use GstBus with GSource for message handling in a GMainLoop. The GstBus marshalls messages from streaming threads to the application thread using GLib's event loop system.

## Key Concepts Demonstrated

1. **GstBus**: Object that delivers messages from streaming threads to application
2. **GSource**: GLib mechanism for integrating external event sources with main loop
3. **gst_bus_add_watch()**: Creates and adds a GSource for the bus to the main context
4. **Message marshalling**: Bus ensures messages are delivered in the main thread
5. **Thread safety**: Streaming happens in separate threads, messages delivered safely

## The GSource Integration Benefits

- Bus messages are delivered efficiently through the GLib event system
- No polling needed - the main loop is notified when messages arrive
- Messages are processed in the main thread context for thread safety
- The application can handle other GSource events (timers, I/O, etc.) seamlessly

## Files

- `gsourceNotification.c` - Main example source code
- `Makefile` - Build configuration for easy compilation

## Compilation Instructions

### Method 1: Using the provided Makefile (Recommended)
```bash
cd /workspaces/cplusplus-devcontainer/src/gstreamer/gstBusHandOn
make
```

### Method 2: Manual compilation
```bash
gcc -o gsource_example gsourceNotification.c `pkg-config --cflags --libs gstreamer-1.0 glib-2.0`
```

### Method 3: Using the workspace build systems
- **Meson**: `ninja -C build`
- **CMake**: `cmake --build buildtests && ./buildtests/runTests`

## Running Instructions

### Basic execution with default pipeline
```bash
./gsource_example
```

### Using custom pipelines
```bash
# Video only (if display is available)
./gsource_example "videotestsrc num-buffers=100 ! autovideosink"

# Audio only (if audio device is available)
./gsource_example "audiotestsrc num-buffers=100 ! autoaudiosink"

# Silent test (no audio/video output)
./gsource_example "audiotestsrc num-buffers=50 ! fakesink"
```

### Using Makefile targets
```bash
make run          # Build and run with default pipeline
make run-video    # Build and run with video-only pipeline
make run-audio    # Build and run with audio-only pipeline
make help         # Show available targets
```

## Example Output

```
Using default test pipeline
You can provide a custom pipeline as argument
Example: ./gsource_example 'videotestsrc ! autovideosink'
Starting pipeline...
Press Ctrl+C to stop
Running main loop...
Pipeline state changed from NULL to READY
Pipeline state changed from READY to PAUSED
Pipeline state changed from PAUSED to PLAYING
End of stream reached.
Main loop finished. Cleaning up...
Goodbye!
```

## Code Structure

### Main Components

1. **AppData Structure**: Holds pipeline, main loop, bus, and watch ID
2. **bus_call()**: Callback function that handles all bus messages
3. **cleanup_app()**: Proper resource cleanup
4. **sigint_handler()**: Graceful shutdown on Ctrl+C
5. **main()**: Initializes GStreamer, creates pipeline, sets up bus watch

### Message Types Handled

- `GST_MESSAGE_EOS`: End of stream
- `GST_MESSAGE_ERROR`: Critical errors
- `GST_MESSAGE_WARNING`: Warning messages
- `GST_MESSAGE_INFO`: Informational messages
- `GST_MESSAGE_STATE_CHANGED`: Pipeline state transitions
- `GST_MESSAGE_BUFFERING`: Buffering progress
- `GST_MESSAGE_CLOCK_LOST`: Clock synchronization issues

### GSource Integration

The key part of the example is this line:
```c
app.bus_watch_id = gst_bus_add_watch (app.bus, bus_call, &app);
```

This creates a GSource for the bus and adds it to the default GLib main context. The alternative manual method is shown in comments:
```c
GSource *bus_source = gst_bus_create_watch (app.bus);
g_source_set_callback (bus_source, (GSourceFunc) bus_call, &app, NULL);
g_source_attach (bus_source, NULL);
g_source_unref (bus_source);
```

## Requirements

- GStreamer 1.0 development packages
- GLib 2.0 development packages
- GCC compiler

On Ubuntu/Debian:
```bash
sudo apt install libgstreamer1.0-dev libglib2.0-dev gcc pkg-config
```

## Troubleshooting

1. **No audio/video output**: Use pipelines with `fakesink` for testing without hardware
2. **Compilation errors**: Ensure GStreamer and GLib development packages are installed
3. **Permission errors**: Make sure the executable has proper permissions
4. **X11 forwarding**: For video output in containers, ensure X11 forwarding is properly configured

## Further Reading

- [GStreamer Bus Documentation](https://gstreamer.freedesktop.org/documentation/gstreamer/gstbus.html)
- [GLib Main Event Loop](https://docs.gtk.org/glib/main-loop.html)
- [GSource Documentation](https://docs.gtk.org/glib/struct.Source.html)