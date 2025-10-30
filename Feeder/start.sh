#!/bin/sh
# input: CSV of pixels interleaved with intersections
# output: pixels and intersections as Messages
BIN=../bin
NAME=Feeder
INPUT_CHANNEL=JUNK
INPUT_SOCKET=tcp://127.0.0.1:1301
OUTPUT_CHANNEL=${1:-"IntersectWith"}
# Primary input file (or first of many)
METADATA=${2:-"data/pixels.txt"}
# Allow overriding the output socket (defaults to XSUB at 1314)
OUTPUT_SOCKET=${3:-tcp://127.0.0.1:1314}

# Support multiple input files: shift the first three processed args so remaining "$@" are extra files only.
shift 3 || true

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA "$@"
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA "$@"
