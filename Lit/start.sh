#!/bin/sh

BIN=../bin
NAME=Lit
INPUT_CHANNEL=LIT
INPUT_SOCKET=tcp://127.0.0.1:1312
OUTPUT_CHANNEL=COLOR
OUTPUT_SOCKET=tcp://127.0.0.1:1303

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
$BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET