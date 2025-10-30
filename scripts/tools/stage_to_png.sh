#!/usr/bin/env bash
# Convert one or more Logger-formatted artifacts (two-line Pixel+Intersection records)
# into an oPNG-shaped file, then render to PNG using existing Writer via Feeder.
#
# Usage: stage_to_png.sh <out.png> <artifact1.txt> [artifact2.txt ...]
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

if [[ $# -lt 2 ]]; then
  echo "Usage: $0 <out.png> <artifact1.txt> [artifact2.txt ...]" >&2
  exit 2
fi

OUT_PNG="$1"; shift
ARTS=("$@")

# Ensure output directory exists and compute absolute output path so Writer (run from its own dir) writes to the intended location
mkdir -p "$(dirname "$OUT_PNG")"
OUT_PNG_ABS="$(cd "$(dirname "$OUT_PNG")" && pwd)/$(basename "$OUT_PNG")"
TMP_OPNG="${TMPDIR:-/tmp}/oPNG.$$.txt"

python3 "$ROOT_DIR/scripts/tools/to_opng.py" "$TMP_OPNG" "${ARTS[@]}"

# Ensure widget libs discoverable when running binaries directly
export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"

# Use fixed, well-known PNG bus from pipeline config to ensure Writer/Feeder rendezvous
PNG_SOCKET="$BUS_PNG"

# Launch Writer bound on the temporary PNG bus
(
  cd "$ROOT_DIR/Writer" && \
  WRITER_BIND_SUB=1 WRITER_INPUT_SOCKET="$PNG_SOCKET" ./start.sh "$OUT_PNG_ABS" > "$TMP_OPNG.log" 2>&1 & echo $! > "$TMP_OPNG.pid"
)
WR_PID=$(cat "$TMP_OPNG.pid")
sleep 0.2

# Feed oPNG to PNG bus
(
  cd "$ROOT_DIR/Feeder" && \
  ./start.sh "$TOPIC_PNG" "$TMP_OPNG" "$PNG_SOCKET" > /dev/null 2>&1 || true
)

for _ in $(seq 1 100); do
  if ! ps -p "$WR_PID" >/dev/null 2>&1; then break; fi
  sleep 0.1
done
if ps -p "$WR_PID" >/dev/null 2>&1; then kill "$WR_PID" >/dev/null 2>&1 || true; fi
rm -f "$TMP_OPNG" "$TMP_OPNG.pid" "$TMP_OPNG.log"

echo "[stage_to_png] Wrote $OUT_PNG_ABS"
