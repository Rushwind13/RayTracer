#!/bin/sh

BIN=../bin
NAME=Logger
INPUT_CHANNEL=LOGGER
INPUT_SOCKET=tcp://127.0.0.1:1300
OUTPUT_CHANNEL=
OUTPUT_SOCKET=

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
$BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
