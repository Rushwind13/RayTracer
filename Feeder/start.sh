#!/bin/sh

BIN=../bin
NAME=Feeder
INPUT_CHANNEL=JUNK
INPUT_SOCKET=tcp://127.0.0.1:1301
OUTPUT_CHANNEL=FEEDER
OUTPUT_SOCKET=tcp://127.0.0.1:1313
METADATA=pixels.txt

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
$BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET $METADATA
