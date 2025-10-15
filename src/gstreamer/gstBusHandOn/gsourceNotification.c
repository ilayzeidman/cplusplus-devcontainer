/*
 * GSource-based GstBus Example
 *
 * This example demonstrates how to use GstBus with GSource for message handling
 * in a GMainLoop. The GstBus marshalls messages from streaming threads to the
 * application thread using GLib's event loop system.
 *
 * Compilation instructions:
 * gcc -o gsource_example gsourceNotification.c `pkg-config --cflags --libs gstreamer-1.0 glib-2.0`
 *
 * Running instructions:
 * ./gsource_example
 *
 * Or use the provided build systems:
 * - Meson: ninja -C build
 * - CMake: cmake --build buildtests && ./buildtests/runTests
 */

#include <gst/gst.h>
#include <glib.h>
#include <glib-unix.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

typedef struct
{
    GstElement *pipeline;
    GMainLoop *loop;
    GstBus *bus;
    guint bus_watch_id;
} AppData;

/* Bus message callback function */
static gboolean
bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
    AppData *app = (AppData *)data;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
        g_print("End of stream reached.\n");
        g_main_loop_quit(app->loop);
        break;

    case GST_MESSAGE_ERROR:
    {
        gchar *debug;
        GError *error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);

        g_printerr("Error: %s\n", error->message);
        g_error_free(error);

        g_main_loop_quit(app->loop);
        break;
    }

    case GST_MESSAGE_WARNING:
    {
        gchar *debug;
        GError *error;

        gst_message_parse_warning(msg, &error, &debug);
        g_free(debug);

        g_print("Warning: %s\n", error->message);
        g_error_free(error);
        break;
    }

    case GST_MESSAGE_INFO:
    {
        gchar *debug;
        GError *error;

        gst_message_parse_info(msg, &error, &debug);
        g_free(debug);

        g_print("Info: %s\n", error->message);
        g_error_free(error);
        break;
    }

    case GST_MESSAGE_STATE_CHANGED:
    {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);

        if (GST_MESSAGE_SRC(msg) == GST_OBJECT(app->pipeline))
        {
            g_print("Pipeline state changed from %s to %s\n",
                    gst_element_state_get_name(old_state),
                    gst_element_state_get_name(new_state));
        }
        break;
    }

    case GST_MESSAGE_BUFFERING:
    {
        gint percent = 0;
        gst_message_parse_buffering(msg, &percent);
        g_print("Buffering (%3d%%)\r", percent);

        /* Wait until buffering is complete before start/resume playing */
        if (percent < 100)
            gst_element_set_state(app->pipeline, GST_STATE_PAUSED);
        else
            gst_element_set_state(app->pipeline, GST_STATE_PLAYING);
        break;
    }

    case GST_MESSAGE_CLOCK_LOST:
        g_print("Clock lost! Restarting playback...\n");
        gst_element_set_state(app->pipeline, GST_STATE_PAUSED);
        gst_element_set_state(app->pipeline, GST_STATE_PLAYING);
        break;

    default:
        /* Uncomment to see all messages */
        /* g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (msg)); */
        break;
    }

    return TRUE; /* Keep the source */
}

/* Cleanup function */
static void
cleanup_app(AppData *app)
{
    if (app->bus_watch_id > 0)
    {
        g_source_remove(app->bus_watch_id);
        app->bus_watch_id = 0;
    }

    if (app->bus)
    {
        gst_object_unref(app->bus);
        app->bus = NULL;
    }

    if (app->pipeline)
    {
        gst_element_set_state(app->pipeline, GST_STATE_NULL);
        gst_object_unref(app->pipeline);
        app->pipeline = NULL;
    }

    if (app->loop)
    {
        g_main_loop_unref(app->loop);
        app->loop = NULL;
    }
}

/* Signal handler for graceful shutdown */
static gboolean
sigint_handler(gpointer data)
{
    AppData *app = (AppData *)data;
    g_print("\nReceived interrupt signal. Shutting down...\n");
    g_main_loop_quit(app->loop);
    return FALSE; /* Remove the source */
}

int main(int argc, char *argv[])
{
    AppData app = {0};
    GstStateChangeReturn ret;
    const char *pipeline_desc;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create a GLib Main Loop */
    app.loop = g_main_loop_new(NULL, FALSE);

    /* Use a test pipeline that generates audio/video */
    pipeline_desc = "videotestsrc num-buffers=300 ! "
                    "video/x-raw,width=320,height=240,framerate=30/1 ! "
                    "videoconvert ! autovideosink "
                    "audiotestsrc num-buffers=300 ! "
                    "audioconvert ! autoaudiosink";

    /* Allow custom pipeline from command line */
    if (argc > 1)
    {
        pipeline_desc = argv[1];
        g_print("Using custom pipeline: %s\n", pipeline_desc);
    }
    else
    {
        g_print("Using default test pipeline\n");
        g_print("You can provide a custom pipeline as argument\n");
        g_print("Example: %s 'videotestsrc ! autovideosink'\n", argv[0]);
    }

    /* Create pipeline from description */
    app.pipeline = gst_parse_launch(pipeline_desc, NULL);
    if (!app.pipeline)
    {
        g_printerr("Failed to create pipeline from description: %s\n", pipeline_desc);
        cleanup_app(&app);
        return -1;
    }

    /* Get the bus from the pipeline */
    app.bus = gst_element_get_bus(app.pipeline);

    /* Add a bus watch to the default GLib main context.
     * This creates a GSource that will be called when messages are available on the bus.
     * The GSource integrates with the GMainLoop for efficient message delivery. */
    app.bus_watch_id = gst_bus_add_watch(app.bus, bus_call, &app);

    /* Alternative method using gst_bus_create_watch():
     * GSource *bus_source = gst_bus_create_watch (app.bus);
     * g_source_set_callback (bus_source, (GSourceFunc) bus_call, &app, NULL);
     * g_source_attach (bus_source, NULL);
     * g_source_unref (bus_source);
     */

    /* Add signal handler for graceful shutdown */
    g_unix_signal_add(SIGINT, sigint_handler, &app);

    g_print("Starting pipeline...\n");
    g_print("Press Ctrl+C to stop\n");

    /* Start playing */
    ret = gst_element_set_state(app.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        cleanup_app(&app);
        return -1;
    }

    /* Run the main loop.
     * This will block until g_main_loop_quit() is called.
     * During this time, the GSource we created will deliver bus messages
     * to our callback function in the main thread context. */
    g_print("Running main loop...\n");
    g_main_loop_run(app.loop);

    g_print("Main loop finished. Cleaning up...\n");

    /* Cleanup */
    cleanup_app(&app);

    g_print("Goodbye!\n");
    return 0;
}

/*
 * Key concepts demonstrated:
 *
 * 1. GstBus: Object that delivers messages from streaming threads to application
 * 2. GSource: GLib mechanism for integrating external event sources with main loop
 * 3. gst_bus_add_watch(): Creates and adds a GSource for the bus to the main context
 * 4. Message marshalling: Bus ensures messages are delivered in the main thread
 * 5. Thread safety: Streaming happens in separate threads, messages delivered safely
 *
 * The GSource integration means:
 * - Bus messages are delivered efficiently through the GLib event system
 * - No polling needed - the main loop is notified when messages arrive
 * - Messages are processed in the main thread context for thread safety
 * - The application can handle other GSource events (timers, I/O, etc.) seamlessly
 */
