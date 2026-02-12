#!/usr/bin/env bash
# Generate aggregated IntersectResults (oIntersectResults.txt) from truth per-object artifacts
# and render intersectresults.png via Writer.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

STAGE_DIR="$ROOT_DIR/runs/truth/stages"
OBJ_DIR="$ROOT_DIR/runs/truth/objects"
mkdir -p "$STAGE_DIR"

# Ensure widget libs discoverable
export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"

# Verify input object artifacts exist
INPUTS=()
for OBJ in "${OBJECTS[@]}"; do
	f="$OBJ_DIR/oIntersectResult.$OBJ.txt"
	if [[ -s "$f" ]]; then INPUTS+=("$f"); else echo "[intersectresults] MISSING: $f" >&2; fi
done
if [[ ${#INPUTS[@]} -eq 0 ]]; then
	echo "[intersectresults] No object artifacts found under $OBJ_DIR" >&2
	exit 1
fi

echo "[intersectresults] Aggregating ${#INPUTS[@]} object files into oIntersectResults.txt"

# Use fixed Shader bus from central config for deterministic aggregation
SH_SOCK="$BUS_SHADER"

# Launch IntersectResults and a logger to capture its Shader-bound output (the aggregated per-pixel intersection)
export LOG_BASEDIR="$STAGE_DIR"
unset LOGGER_BIND # ensure logger connects to the temp socket
(
	cd "$ROOT_DIR/IntersectResults" && INTERSECTRESULTS_OUTPUT_SOCKET="$SH_SOCK" ./start.sh >"$STAGE_DIR/intersectresults.log" 2>&1 & echo $! >"$STAGE_DIR/ir.pid"
)
sleep 0.25
(
	cd "$ROOT_DIR/Logger" && ./start.sh "$SH_SOCK" "$TOPIC_SHADER" IntersectResults.txt >"$STAGE_DIR/logger_ir.log" 2>&1 & echo $! >"$STAGE_DIR/logger_ir.pid"
)
sleep 0.25

# Feed all object artifacts into IntersectResults (EOF after the last file)
(
	cd "$ROOT_DIR/Feeder" && ./start.sh "$TOPIC_INTERSECT_RESULT" "${INPUTS[0]}" "$BUS_INTERSECT_RESULT" "${INPUTS[@]:1}" >"$STAGE_DIR/feeder_ir.log" 2>&1 || true
)

# Wait for logger to finish (EOF) up to 20s
if [[ -f "$STAGE_DIR/logger_ir.pid" ]]; then
	for _ in $(seq 1 200); do
		if ! ps -p "$(cat "$STAGE_DIR/logger_ir.pid")" >/dev/null 2>&1; then break; fi
		sleep 0.1
	done
	# If still running, stop it
	if ps -p "$(cat "$STAGE_DIR/logger_ir.pid")" >/dev/null 2>&1; then kill "$(cat "$STAGE_DIR/logger_ir.pid")" 2>/dev/null || true; fi
fi
if [[ -f "$STAGE_DIR/ir.pid" ]]; then kill "$(cat "$STAGE_DIR/ir.pid")" 2>/dev/null || true; fi
unset LOG_BASEDIR LOGGER_BIND

# Render PNG for visualization
if [[ -s "$STAGE_DIR/oIntersectResults.txt" ]]; then
	"$ROOT_DIR/scripts/tools/stage_to_png.sh" "$STAGE_DIR/intersectresults.png" "$STAGE_DIR/oIntersectResults.txt" >"$STAGE_DIR/writer_ir.log" 2>&1 || true
	echo "[intersectresults] Wrote: $STAGE_DIR/oIntersectResults.txt and $STAGE_DIR/intersectresults.png"
else
	echo "[intersectresults] ERROR: Missing aggregated output: $STAGE_DIR/oIntersectResults.txt" >&2
	exit 2
fi
