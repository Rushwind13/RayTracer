#!/bin/sh
# input: pixels (blank intersections) from PixelFactory
# output: pixels and intersections to IntersectResults
BIN=../bin
NAME=IntersectWith
INPUT_CHANNEL=IntersectWith
INPUT_SOCKET=tcp://127.0.0.1:1313
OUTPUT_CHANNEL=IntersectResult
OUTPUT_SOCKET=tcp://127.0.0.1:1300
METADATA=$1 # Note: You need to pass a world object for this to watch

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
