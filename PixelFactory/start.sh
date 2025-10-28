#!/bin/sh
# input: none
# output: pixels and (blank) intersections to IntersectWith
BIN=../bin
NAME=PixelFactory
INPUT_CHANNEL=
INPUT_SOCKET=
OUTPUT_CHANNEL=IntersectWith
# OUTPUT_SOCKET=ipc:///tmp/feeds/intersect
OUTPUT_SOCKET=tcp://127.0.0.1:1314
echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
