#!/bin/sh
# input: pixels (blank intersections) from PixelFactory
# output: pixels and intersections to IntersectResults
BIN=../bin
NAME=IntersectWith
METADATA=$1 # Note: You need to pass a world object for this to watch
INPUT_SOCKET=${2:-tcp://127.0.0.1:1313}
OUTPUT_CHANNEL=${3:-IntersectResult}
OUTPUT_SOCKET=${4:-tcp://127.0.0.1:1314}
INPUT_CHANNEL=${INTERSECTWITH_INPUT_CHANNEL:-IntersectWith}

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
