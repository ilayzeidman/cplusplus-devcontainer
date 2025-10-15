# Simple Makefile for GSource GstBus example
CC = gcc
CFLAGS = -Wall -g
PKGS = gstreamer-1.0 glib-2.0

# Get compiler flags from pkg-config
CFLAGS += $(shell pkg-config --cflags $(PKGS))
LIBS = $(shell pkg-config --libs $(PKGS))

TARGET = gsource_example
SOURCE = gsourceNotification.c

# Default target
all: $(TARGET)

# Build the example
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Run the example
run: $(TARGET)
	./$(TARGET)

# Run with a simple video-only pipeline
run-video: $(TARGET)
	./$(TARGET) "videotestsrc num-buffers=100 ! autovideosink"

# Run with a simple audio-only pipeline
run-audio: $(TARGET)
	./$(TARGET) "audiotestsrc num-buffers=100 ! autoaudiosink"

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build the example (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  run       - Build and run with default pipeline"
	@echo "  run-video - Build and run with video-only pipeline"
	@echo "  run-audio - Build and run with audio-only pipeline"
	@echo "  help      - Show this help message"

.PHONY: all clean run run-video run-audio help