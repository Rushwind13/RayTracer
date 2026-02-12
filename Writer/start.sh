#!/bin/sh

BIN=../bin
NAME=Writer
INPUT_CHANNEL=PNG
# Allow overriding the input socket via env var to avoid bind conflicts in isolated runs
INPUT_SOCKET=${WRITER_INPUT_SOCKET:-tcp://127.0.0.1:1308}
# Write output PNG into the top-level bin directory so tests can find it
# Allow override by providing an absolute or relative path as $1
METADATA=${1:-../bin/test.png}

echo $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $METADATA
exec $BIN/$NAME $NAME $INPUT_CHANNEL $INPUT_SOCKET $METADATA
