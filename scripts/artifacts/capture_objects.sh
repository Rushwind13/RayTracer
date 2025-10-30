#!/usr/bin/env bash
# Capture per-object IntersectWith artifacts for specified objects into runs/truth/objects
# Usage: capture_objects.sh <obj1> [obj2 ...]
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <obj1> [obj2 ...]" >&2
  exit 2
fi

TRUTH_DIR="$ROOT_DIR/runs/truth"
OBJ_DIR="$TRUTH_DIR/objects"
mkdir -p "$OBJ_DIR"

# Ensure widget libs discoverable
export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"

# Start proxy for INTERSECT fan-out
"$ROOT_DIR/../zmq_widgets/bin/ControlChannel" "$PROXY_XSUB" "$PROXY_XPUB" >/dev/null 2>&1 &
PROXY_PID=$!
sleep 0.2

launch() { ( cd "$1" && shift && "$@" ) >/dev/null 2>&1 & echo $!; }

for OBJ in "$@"; do
  echo "[capture] $OBJ"
  export LOG_BASEDIR="$OBJ_DIR"
  export LOGGER_BIND=1
  rm -f "$OBJ_DIR/oIntersectResult.$OBJ.txt"
  L_PID=$(launch "$ROOT_DIR/Logger" ./start.sh "$BUS_INTERSECT_RESULT" "$TOPIC_INTERSECT_RESULT" "IntersectResult.$OBJ.txt")
  sleep 0.15
  IW_PID=$(launch "$ROOT_DIR/IntersectWith" ./start.sh "$OBJ" "$BUS_INTERSECT_RESULT")
  sleep 0.15
  ( cd "$ROOT_DIR/Feeder" && ./start.sh IntersectWith "$ROOT_DIR/data/pixels.txt" "$PROXY_XSUB" ) >/dev/null 2>&1 || true
  sleep 0.2; kill "$IW_PID" >/dev/null 2>&1 || true; sleep 0.1; kill -9 "$IW_PID" >/dev/null 2>&1 || true
  sleep 0.2; kill "$L_PID" >/dev/null 2>&1 || true
  unset LOG_BASEDIR LOGGER_BIND
done

kill "$PROXY_PID" >/dev/null 2>&1 || true

echo "[capture] done"
