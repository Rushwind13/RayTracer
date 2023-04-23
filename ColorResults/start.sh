#!/bin/sh

BIN=../bin
NAME=ColorResults
INPUT_CHANNEL=COLOR
INPUT_SOCKET=tcp://127.0.0.1:1303
OUTPUT_CHANNEL=DEPTH
OUTPUT_SOCKET=tcp://127.0.0.1:1307

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
$BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
