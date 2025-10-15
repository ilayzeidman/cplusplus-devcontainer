#include <glib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

typedef struct
{
    GSource source;
    GPollFD pfd; // fd + events + revents
} PipeSource;

static gboolean pipe_prepare(GSource *s, int *timeout_ms)
{
    *timeout_ms = -1; // no custom timeout; sleep until fd readies
    return FALSE;
}

static gboolean pipe_check(GSource *s)
{
    PipeSource *ps = (PipeSource *)s;
    return (ps->pfd.revents & G_IO_IN) != 0;
}

static gboolean pipe_dispatch(GSource *s, GSourceFunc cb, gpointer data)
{
    PipeSource *ps = (PipeSource *)s;
    char buf[256];
    ssize_t n = read(ps->pfd.fd, buf, sizeof buf); // drain/read available data

    if (n > 0)
    {
        buf[n - 1] = '\0'; // null terminate (remove newline)
        g_print("Read from pipe: %s\n", buf);

        // Check for quit command
        if (strcmp(buf, "quit") == 0)
        {
            g_print("Received quit command, stopping main loop\n");
            GMainLoop *loop = (GMainLoop *)data;
            g_main_loop_quit(loop);
            return FALSE;
        }
    }
    else if (n == 0)
    {
        g_print("Pipe closed\n");
        return FALSE; // remove source
    }
    else
    {
        g_print("Read error: %s\n", strerror(errno));
        return FALSE; // remove source
    }

    return cb ? cb(data) : TRUE;
}

static void pipe_finalize(GSource *s)
{
    PipeSource *ps = (PipeSource *)s;
    g_print("Finalizing pipe source\n");
    // If your source owns the fd, close it here; if not, skip.
    // close(ps->pfd.fd);
}

static GSourceFuncs pipe_funcs = {
    .prepare = pipe_prepare,
    .check = pipe_check,
    .dispatch = pipe_dispatch,
    .finalize = pipe_finalize};

static GSource *pipe_source_new(int fd)
{
    PipeSource *ps = (PipeSource *)g_source_new(&pipe_funcs, sizeof *ps);
    ps->pfd.fd = fd;
    ps->pfd.events = G_IO_IN;
    ps->pfd.revents = 0;
    g_source_add_poll((GSource *)ps, &ps->pfd); // or g_source_add_unix_fd(..., G_IO_IN)
    return (GSource *)ps;
}

// Callback function when pipe is ready
static gboolean on_pipe_ready(gpointer data)
{
    g_print("Pipe is ready for reading\n");
    return TRUE;
}

// Signal handler for graceful shutdown
static GMainLoop *main_loop = NULL;

static void signal_handler(int sig)
{
    g_print("\nReceived signal %d, shutting down...\n", sig);
    if (main_loop)
    {
        g_main_loop_quit(main_loop);
    }
}

int main(int argc, char *argv[])
{
    int pipe_fds[2];
    GSource *pipe_source;
    guint source_id;

    g_print("FD-driven GSource Example\n");
    g_print("========================\n\n");

    // Create a pipe
    if (pipe(pipe_fds) == -1)
    {
        g_error("Failed to create pipe: %s", strerror(errno));
        return 1;
    }

    g_print("Created pipe: read_fd=%d, write_fd=%d\n", pipe_fds[0], pipe_fds[1]);

    // Create main loop
    main_loop = g_main_loop_new(NULL, FALSE);

    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Create and attach our custom pipe source
    pipe_source = pipe_source_new(pipe_fds[0]);     // monitor read end
    source_id = g_source_attach(pipe_source, NULL); // attach to default context

    // Set the callback (optional, since dispatch handles the logic)
    g_source_set_callback(pipe_source, on_pipe_ready, main_loop, NULL);

    g_print("Pipe source created and attached (ID: %u)\n", source_id);
    g_print("\nTo test the pipe, run in another terminal:\n");
    g_print("  echo 'hello' > /proc/%d/fd/%d\n", getpid(), pipe_fds[1]);
    g_print("  echo 'world' > /proc/%d/fd/%d\n", getpid(), pipe_fds[1]);
    g_print("  echo 'quit' > /proc/%d/fd/%d\n", getpid(), pipe_fds[1]);
    g_print("\nOr use the helper script (if available):\n");
    g_print("  ./send_to_pipe.sh %d\n", pipe_fds[1]);
    g_print("\nPress Ctrl+C to exit\n\n");

    // Run the main loop
    g_main_loop_run(main_loop);

    // Cleanup
    g_print("\nCleaning up...\n");
    g_source_destroy(pipe_source);
    g_source_unref(pipe_source);
    g_main_loop_unref(main_loop);

    close(pipe_fds[0]);
    close(pipe_fds[1]);

    g_print("Example completed successfully\n");
    return 0;
}