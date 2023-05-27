#!/bin/sh
# input: pixels (blank intersections) from IntersectWith (per world object)
# output: pixels and intersections to {SHADE/BKG || BLACK/LIT (shadow test)}
BIN=../bin
NAME=IntersectResults
INPUT_CHANNEL=RESULT
INPUT_SOCKET=tcp://127.0.0.1:1301
OUTPUT_CHANNEL=SHADE
OUTPUT_SOCKET=tcp://127.0.0.1:1312

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
$BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
