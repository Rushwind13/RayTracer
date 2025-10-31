#!/bin/sh
# input: pixels and intersections as Messages
# output: identical messages to a new channel/socket
BIN=../bin
NAME=Echo
# Args: [input_socket] [topic_to_subscribe] [output_channel] [output_socket]
INPUT_SOCKET=${1:-tcp://127.0.0.1:1300}
INPUT_CHANNEL=${2:-}
OUTPUT_CHANNEL=${3:-$INPUT_CHANNEL}
OUTPUT_SOCKET=${4:-tcp://127.0.0.1:1301}

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
