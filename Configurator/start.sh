#!/bin/sh

BIN=../bin
NAME=Configurator
INPUT_CHANNEL=
INPUT_SOCKET=
OUTPUT_CHANNEL=PNG
OUTPUT_SOCKET=tcp://127.0.0.1:1308

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
$BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $OUTPUT_CHANNEL $OUTPUT_SOCKET
