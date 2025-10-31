#!/bin/sh
# input: aggregated IntersectResults messages (Pixel + Intersection)
# output: routed messages to Shader/Background/Black/Lit topics on the publication socket
BIN=../bin
NAME=Mailroom
# Defaults align with central config but can be overridden
INPUT_CHANNEL=${2:-IntersectResults}
INPUT_SOCKET=${1:-tcp://127.0.0.1:1312}
OUTPUT_CHANNEL=${3:-}
OUTPUT_SOCKET=${4:-tcp://127.0.0.1:1312}

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
