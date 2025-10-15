#!/bin/bash
if [ $# -ne 1 ]; then
    echo "Usage: $0 <write_fd>"
    exit 1
fi
WRITE_FD=$1
echo "Sending test messages to pipe..."
echo "hello" > /proc/$(pgrep -f custom_source_fd)/fd/$WRITE_FD
sleep 1
echo "world" > /proc/$(pgrep -f custom_source_fd)/fd/$WRITE_FD
sleep 1
echo "testing fd source" > /proc/$(pgrep -f custom_source_fd)/fd/$WRITE_FD
sleep 1
echo "quit" > /proc/$(pgrep -f custom_source_fd)/fd/$WRITE_FD
