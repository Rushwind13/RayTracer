#!/bin/sh
# input: CSV of pixels interleaved with intersections
# output: pixels and intersections as Messages
BIN=../bin
NAME=Feeder
INPUT_CHANNEL=JUNK
INPUT_SOCKET=tcp://127.0.0.1:1301
OUTPUT_CHANNEL=${1:-"IntersectWith"}
OUTPUT_SOCKET=tcp://127.0.0.1:1314
METADATA=${2:-"data/pixels.txt"}

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
