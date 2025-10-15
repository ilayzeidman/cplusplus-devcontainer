# Makefile for FD-driven GSource example
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
GLIB_CFLAGS = $(shell pkg-config --cflags glib-2.0)
GLIB_LIBS = $(shell pkg-config --libs glib-2.0)

# Target executable
TARGET = custom_source_fd
SOURCE = custom_source_fd.c
HELPER_SCRIPT = send_to_pipe.sh

.PHONY: all clean run test help

all: $(TARGET) $(HELPER_SCRIPT)

$(TARGET): $(SOURCE)
	@echo "Compiling $(TARGET)..."
	$(CC) $(CFLAGS) $(GLIB_CFLAGS) -o $@ $< $(GLIB_LIBS)
	@echo "Build successful!"

$(HELPER_SCRIPT):
	@echo "Creating helper script..."
	@echo '#!/bin/bash' > $@
	@echo 'if [ $$# -ne 1 ]; then' >> $@
	@echo '    echo "Usage: $$0 <write_fd>"' >> $@
	@echo '    exit 1' >> $@
	@echo 'fi' >> $@
	@echo 'WRITE_FD=$$1' >> $@
	@echo 'echo "Sending test messages to pipe..."' >> $@
	@echo 'echo "hello" > /proc/$$(pgrep -f custom_source_fd)/fd/$$WRITE_FD' >> $@
	@echo 'sleep 1' >> $@
	@echo 'echo "world" > /proc/$$(pgrep -f custom_source_fd)/fd/$$WRITE_FD' >> $@
	@echo 'sleep 1' >> $@
	@echo 'echo "testing fd source" > /proc/$$(pgrep -f custom_source_fd)/fd/$$WRITE_FD' >> $@
	@echo 'sleep 1' >> $@
	@echo 'echo "quit" > /proc/$$(pgrep -f custom_source_fd)/fd/$$WRITE_FD' >> $@
	@chmod +x $@

clean:
	@echo "Cleaning..."
	rm -f $(TARGET) $(HELPER_SCRIPT)
	@echo "Clean complete!"

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

test: $(TARGET) $(HELPER_SCRIPT)
	@echo "Starting test..."
	@echo "Run './$(TARGET)' in one terminal, then './$(HELPER_SCRIPT) <write_fd>' in another"

help:
	@echo "Available targets:"
	@echo "  all     - Build the executable and helper script"
	@echo "  clean   - Remove built files"
	@echo "  run     - Build and run the program"
	@echo "  test    - Build and show test instructions"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Manual testing:"
	@echo "1. Run 'make run' or './$(TARGET)'"
	@echo "2. Note the pipe file descriptors shown"
	@echo "3. In another terminal, send data:"
	@echo "   echo 'hello' > /proc/\$$(pgrep -f $(TARGET))/fd/<write_fd>"
	@echo "4. Send 'quit' to stop the program"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@pkg-config --exists glib-2.0 || (echo "ERROR: glib-2.0 not found. Install with: sudo apt-get install libglib2.0-dev" && false)
	@echo "Dependencies OK!"

.PHONY: check-deps
all: check-deps