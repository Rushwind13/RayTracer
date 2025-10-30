#!/bin/sh
# input: pixels (blank intersections) from IntersectWith (per world object)
# output: pixels and intersections to {SHADE/BKG || BLACK/LIT (shadow test)}
BIN=../bin
NAME=IntersectResults
INPUT_CHANNEL=IntersectResult
# Allow socket overrides to avoid port conflicts in stepwise/isolated runs
INPUT_SOCKET=${INTERSECTRESULTS_INPUT_SOCKET:-tcp://127.0.0.1:1300}
# Align with Shader subscriber topic
OUTPUT_CHANNEL=Shader
OUTPUT_SOCKET=${INTERSECTRESULTS_OUTPUT_SOCKET:-tcp://127.0.0.1:1312}

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
