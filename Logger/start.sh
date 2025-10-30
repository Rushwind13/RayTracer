#!/bin/sh
# input: pixels and intersections as Messages
# output: CSV of pixels interleaved with intersections
BIN=../bin
NAME=Logger
# Args: [input_socket] [topic_to_subscribe] [output_label]
INPUT_SOCKET=${1:-tcp://127.0.0.1:1300}
INPUT_CHANNEL=${2:-}
OUTPUT_CHANNEL=${3:-}
OUTPUT_SOCKET=

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL
