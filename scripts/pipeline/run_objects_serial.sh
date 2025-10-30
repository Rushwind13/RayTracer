#!/usr/bin/env bash
# Run IntersectWith one object at a time, capturing per-object IntersectResult artifacts,
# then feed all artifacts serially into IntersectResults (depth sort) and run downstream to PNG.
# Also render per-artifact PNG frames (presence map with purple for missing).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

RUN_ID=${RUN_ID:-objects-$(date +%Y%m%d-%H%M%S)}
RUN_DIR="$ROOT_DIR/runs/$RUN_ID"
mkdir -p "$RUN_DIR"
echo "[objects] Run directory: $RUN_DIR"

# Ensure widget libs discoverable
export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"

# Start proxy for INTERSECT input fan-out
"$ROOT_DIR/../zmq_widgets/bin/ControlChannel" "$PROXY_XSUB" "$PROXY_XPUB" >"$RUN_DIR/proxy.log" 2>&1 &
PROXY_PID=$!
sleep 0.2

# Helper to launch a component in a directory
launch() {
	local dir=$1; shift
	local name=$1; shift
	( cd "$dir" && "$@" ) >"$RUN_DIR/${name}.log" 2>&1 &
	echo $!
}

# 1) Per-object IntersectWith runs
OBJ_FILES=()
for OBJ in "${OBJECTS[@]}"; do
	echo "[objects] === Object: $OBJ ==="
	OBJ_DIR="$RUN_DIR/$OBJ"
	mkdir -p "$OBJ_DIR"
	export LOG_BASEDIR="$OBJ_DIR"
	export LOGGER_BIND=1 # Bind SUB on 1300 so IW can connect
	# Logger for IntersectResult
	LOG_PID=$(launch "$ROOT_DIR/Logger" "logger_${OBJ}" ./start.sh "$BUS_INTERSECT_RESULT" "$TOPIC_INTERSECT_RESULT" "IntersectResult.$OBJ.txt")
	sleep 0.15
	# IntersectWith for this object
	IW_PID=$(launch "$ROOT_DIR/IntersectWith" "intersect_with_${OBJ}" ./start.sh "$OBJ" "$BUS_INTERSECT_RESULT")
	sleep 0.15
	# Feed pixels to INTERSECT bus (via proxy XSUB)
	export FEEDER_LIMIT=-1
	( cd "$ROOT_DIR/Feeder" && ./start.sh IntersectWith "$ROOT_DIR/data/pixels.txt" "$PROXY_XSUB" ) >"$OBJ_DIR/feeder_${OBJ}.log" 2>&1 || true
	# Wait a moment and terminate IntersectWith
	sleep 0.2; kill "$IW_PID" 2>/dev/null || true; sleep 0.1; kill -9 "$IW_PID" 2>/dev/null || true
	# Ensure logger flushed
	sleep 0.2
	# Render per-object artifact to PNG using Writer (no PIL)
	OBJ_FILE="$OBJ_DIR/oIntersectResult.$OBJ.txt"
	if [[ -s "$OBJ_FILE" ]]; then
		"$ROOT_DIR/scripts/tools/stage_to_png.sh" "$OBJ_DIR/${OBJ}.png" "$OBJ_FILE" || true
		OBJ_FILES+=("$OBJ_FILE")
	else
		echo "[objects] WARN: missing artifact for $OBJ: $OBJ_FILE"
	fi
	unset LOG_BASEDIR LOGGER_BIND
	echo "[objects] Done: $OBJ"
	sleep 0.2
done

# 2) Run IntersectResults + Background + Shader to produce COLOR directly from per-object files
export LOG_BASEDIR="$RUN_DIR"
IR_PID=$(launch "$ROOT_DIR/IntersectResults" "intersect_results" ./start.sh)
BG_PID=$(launch "$ROOT_DIR/Background" "background" ./start.sh)
SH_PID=$(launch "$ROOT_DIR/Shader" "shader" ./start.sh)
sleep 0.2
logger3=$(launch "$ROOT_DIR/Logger" "logger_stage3" ./start.sh "$BUS_COLOR" "$TOPIC_COLOR" COLOR.txt)
sleep 0.15
# Feed all object files in one Feeder invocation (EOF only after last)
( cd "$ROOT_DIR/Feeder" && ./start.sh "$TOPIC_INTERSECT_RESULT" "${OBJ_FILES[0]}" "$BUS_INTERSECT_RESULT" "${OBJ_FILES[@]:1}" ) >"$RUN_DIR/feeder_stage2.log" 2>&1 || true
sleep 0.5; kill "$IR_PID" "$BG_PID" "$SH_PID" 2>/dev/null || true
sleep 0.2

# 4) DEPTH: ColorResults → DEPTH, capture
CR_PID=$(launch "$ROOT_DIR/ColorResults" "colorresults" ./start.sh)
sleep 0.15
logger4=$(launch "$ROOT_DIR/Logger" "logger_stage4" ./start.sh "$BUS_DEPTH" "$TOPIC_DEPTH" DEPTH.txt)
sleep 0.15
( cd "$ROOT_DIR/Feeder" && ./start.sh "$TOPIC_COLOR" "$RUN_DIR/oCOLOR.txt" "$BUS_COLOR" ) >"$RUN_DIR/feeder_stage4.log" 2>&1 || true
sleep 0.5; kill "$CR_PID" 2>/dev/null || true
sleep 0.2

# 5) PNG: DepthChart → PNG, capture
DC_PID=$(launch "$ROOT_DIR/DepthChart" "depthchart" ./start.sh)
sleep 0.15
logger5=$(launch "$ROOT_DIR/Logger" "logger_stage5" ./start.sh "$BUS_PNG" "$TOPIC_PNG" PNG.txt)
sleep 0.15
( cd "$ROOT_DIR/Feeder" && ./start.sh "$TOPIC_DEPTH" "$RUN_DIR/oDEPTH.txt" "$BUS_DEPTH" ) >"$RUN_DIR/feeder_stage5.log" 2>&1 || true
sleep 0.5; kill "$DC_PID" 2>/dev/null || true
sleep 0.2

# 6) Final: Render test.png via Writer (wait for natural exit)
"$ROOT_DIR/scripts/tools/stage_to_png.sh" "$RUN_DIR/test.png" "$RUN_DIR/oPNG.txt" >"$RUN_DIR/writer.log" 2>&1 || true

# Stop proxy
kill "$PROXY_PID" 2>/dev/null || true

# Render stage artifacts to PNG using Writer
if [[ -s "$RUN_DIR/oCOLOR.txt" ]]; then "$ROOT_DIR/scripts/tools/stage_to_png.sh" "$RUN_DIR/frame_color.png" "$RUN_DIR/oCOLOR.txt" || true; fi
if [[ -s "$RUN_DIR/oDEPTH.txt" ]]; then "$ROOT_DIR/scripts/tools/stage_to_png.sh" "$RUN_DIR/frame_depth.png" "$RUN_DIR/oDEPTH.txt" || true; fi
if [[ -s "$RUN_DIR/oPNG.txt" ]]; then "$ROOT_DIR/scripts/tools/stage_to_png.sh" "$RUN_DIR/frame_png.png" "$RUN_DIR/oPNG.txt" || true; fi

echo "[objects] Artifacts in: $RUN_DIR"
"$ROOT_DIR/scripts/validation/pretty_logs.sh" "$RUN_DIR" -n 25 || true
