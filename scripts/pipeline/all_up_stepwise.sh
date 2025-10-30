#!/usr/bin/env bash
# Run the ray tracer pipeline in stepwise mode: execute one stage at a time,
# logging outputs to files and feeding them to the next stage.
# MacOS bash-compatible.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
BIN_DIR="$ROOT_DIR/bin"
LOG_PARENT="$ROOT_DIR/runs"
RUN_ID="${RUN_ID:-$(date +%Y%m%d-%H%M%S)}"
RUN_DIR="$LOG_PARENT/$RUN_ID"
mkdir -p "$RUN_DIR"

# Load centralized pipeline configuration (topics, sockets)
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

# Ensure zmq_widgets dynamic library is discoverable on macOS
export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"

# In stepwise, default to SMOKE_MODE=1 for quick aggregator completion unless overridden.
# For truth-like runs (unlimited feed), automatically disable smoke mode.
export SMOKE_MODE=${SMOKE_MODE:-1}
# Feed a minimal number of records by default for smoke (override with STEPWISE_FEEDER_LIMIT)
export FEEDER_LIMIT=${STEPWISE_FEEDER_LIMIT:-1}

echo "[stepwise] Run directory: $RUN_DIR"

# Ensure we validate fresh PNG output by removing any prior file
PNG_FILE="$ROOT_DIR/bin/test.png"
rm -f "$PNG_FILE" 2>/dev/null || true

# Timeouts for stage waits (seconds); override with env if needed.
# Fan-in and accumulation steps can take longer. Favor generous defaults for stages 2–5.
if [[ "${FEEDER_LIMIT:-1}" == "1" ]]; then
	STAGE1_TIMEOUT=${STAGE1_TIMEOUT:-20}
	STAGE_DEFAULT_TIMEOUT=${STAGE_DEFAULT_TIMEOUT:-60}
else
	STAGE1_TIMEOUT=${STAGE1_TIMEOUT:-300}
	STAGE_DEFAULT_TIMEOUT=${STAGE_DEFAULT_TIMEOUT:-600}
	export SMOKE_MODE=0
fi
# Use long defaults for fan-in stages unless caller overrides explicitly
STAGE2_TIMEOUT=${STAGE2_TIMEOUT:-900}
STAGE3_TIMEOUT=${STAGE3_TIMEOUT:-900}
STAGE4_TIMEOUT=${STAGE4_TIMEOUT:-900}
STAGE5_TIMEOUT=${STAGE5_TIMEOUT:-900}

global_pids=()
stage_pids=()
cleanup_stage() {
	echo "\n[stepwise] Cleaning up..."
	for pid in "${stage_pids[@]:-}"; do
		if kill -0 "$pid" 2>/dev/null; then
			kill "$pid" 2>/dev/null || true
		fi
	done
	sleep 0.2
	for pid in "${stage_pids[@]:-}"; do
		if kill -0 "$pid" 2>/dev/null; then
			kill -9 "$pid" 2>/dev/null || true
		fi
	done
	stage_pids=()
}

cleanup_all() {
	cleanup_stage || true
	# Now cleanup globals (e.g., proxy)
	for pid in "${global_pids[@]:-}"; do
		if kill -0 "$pid" 2>/dev/null; then
			kill "$pid" 2>/dev/null || true
		fi
	done
	sleep 0.2
	for pid in "${global_pids[@]:-}"; do
		if kill -0 "$pid" 2>/dev/null; then
			kill -9 "$pid" 2>/dev/null || true
		fi
	done
}
trap cleanup_all EXIT INT TERM

launch() {
	local name=$1; shift
	echo "[stepwise] starting $name"
	"$@" >"$RUN_DIR/${name}.log" 2>&1 &
	LAST_PID=$!
	stage_pids+=("$LAST_PID")
}

run_in() {
	local dir=$1; shift
	pushd "$dir" >/dev/null
	launch "$@"
	popd >/dev/null
}

wait_and_clear_pids() {
	# Wait for the last started pid (typically the Logger) to complete this stage
	local last_index=$((${#stage_pids[@]} - 1))
	if (( last_index >= 0 )); then
		local wait_pid=${stage_pids[$last_index]}
		if [[ -n "${wait_pid:-}" ]]; then
			wait "$wait_pid" || true
		fi
	fi
	# Then cleanup remaining for this stage
	cleanup_stage || true
}

# Wait for a specific PID up to a timeout; returns 0 if finished, 124 if timed out
wait_for_pid_with_timeout() {
	local pid=$1
	local timeout=$2
	local waited=0
	while kill -0 "$pid" 2>/dev/null; do
		sleep 1
		waited=$((waited+1))
		if (( waited >= timeout )); then
			echo "[stepwise] WARN: timeout waiting for pid $pid after ${timeout}s"
			return 124
		fi
	done
	return 0
}

# Utility: return 0 (true) if file exists and has non-zero size
has_data() {
	local f=$1
	[[ -s "$f" ]]
}

# Start XPUB/XSUB proxy for INTERSECT bus once for the entire run
echo "[stepwise] starting proxy"
"$ROOT_DIR/../zmq_widgets/bin/ControlChannel" "$PROXY_XSUB" "$PROXY_XPUB" >"$RUN_DIR/proxy.log" 2>&1 &
global_pids+=("$!")
sleep 0.2

# Helper to run a stage consisting of: [optional workers], aggregator/worker, logger, feeder.
# Arguments:
# 1: stage name
# 2: logger socket (where to subscribe)
# 3: logger label (file label)
# 4: feeder output channel (topic label)
# 5: feeder metadata file (input file)
# 6: feeder output socket (where to publish)
# Remaining args: commands to launch before logger/feeder (e.g., workers or aggregators), one per line as "dir name ./start.sh [args]"
run_stage() {
	local stage_name=$1; shift
	local logger_socket=$1; shift
	local logger_label=$1; shift
	local feeder_channel=$1; shift
	local feeder_file=$1; shift
	local feeder_socket=$1; shift

	echo "\n[stepwise] === Stage: $stage_name ==="

	# Launch any provided pre-logger/feeder processes (workers/aggregators)
	while (( "$#" )); do
		local dir=$1; shift
		local comp_name=$1; shift
		local cmd=$1; shift
		# capture optional remaining args for this command until a marker "--" or next triplet; to keep simple, assume triplets only
		run_in "$dir" "$comp_name" $cmd "$@"
		break
	done

	# Launch Logger with runtime output base dir
	export LOG_BASEDIR="$RUN_DIR"
	run_in "$ROOT_DIR/Logger" "logger_${stage_name}" ./start.sh "$logger_socket" "$logger_label"

	# Launch Feeder to drive input into this stage
	run_in "$ROOT_DIR/Feeder" "feeder_${stage_name}" ./start.sh "$feeder_channel" "$feeder_file" "$feeder_socket"

	# Wait for logger to finish (EOF) and then cleanup stage processes
	wait_and_clear_pids
}

# Stage 1: IntersectWith workers; capture IntersectResult
echo "[stepwise] Stage 1: IntersectWith -> IntersectResult"
RESUME_STAGE=${RESUME_STAGE:-1}
RESUME_STAGE1_FILE=${RESUME_STAGE1_FILE:-}
RESUME_APPEND=${RESUME_APPEND:-0}
# Defaults to runs/complete for resume files if not explicitly provided
COMPLETE_DIR="$ROOT_DIR/runs/complete"
if [[ -z "$RESUME_STAGE1_FILE" && -f "$COMPLETE_DIR/oIntersectResult.txt" ]]; then
	RESUME_STAGE1_FILE="$COMPLETE_DIR/oIntersectResult.txt"
fi
if [[ "$RESUME_STAGE" -ge 2 && -z "${STEPWISE_FEEDER_LIMIT:-}" ]]; then
	# When resuming from Stage 2+, default to unlimited feed unless explicitly overridden
	export FEEDER_LIMIT=-1
	# Full feed implies non-smoke
	export SMOKE_MODE=0
	# Ensure timeouts reflect full run sizes
	STAGE_DEFAULT_TIMEOUT=${STAGE_DEFAULT_TIMEOUT:-600}
	STAGE2_TIMEOUT=${STAGE2_TIMEOUT:-900}
	STAGE3_TIMEOUT=${STAGE3_TIMEOUT:-900}
	STAGE4_TIMEOUT=${STAGE4_TIMEOUT:-900}
	STAGE5_TIMEOUT=${STAGE5_TIMEOUT:-900}
fi
if [[ "$RESUME_STAGE" -le 1 ]]; then
	for OBJ in "${OBJECTS[@]}"; do
		# Publish IntersectResult via the XPUB/XSUB proxy (XSUB at 1314), allowing Logger to fan-out subscribe at 1313
		run_in "$ROOT_DIR/IntersectWith" "intersect_with_${OBJ}" ./start.sh "$OBJ" "$PROXY_XSUB"
	done
	export LOG_BASEDIR="$RUN_DIR"
	if [[ "$RESUME_APPEND" != "0" && -n "$RESUME_STAGE1_FILE" ]]; then
		# Append to an existing file when resuming Stage 1
		export LOG_APPEND=1
	fi
	run_in "$ROOT_DIR/Logger" "logger_stage1" ./start.sh "$PROXY_XPUB" "$TOPIC_INTERSECT_RESULT" IntersectResult.txt
	# Give subscribers a brief moment to join before feeding
	sleep 0.15
	# Remember logger PID for this stage, so we wait for completion (EOF) before cleanup
	LOGGER_PID=$LAST_PID
	STEPWISE_INPUT_FILE=${STEPWISE_INPUT_FILE:-"$ROOT_DIR/data/pixels.txt"}
	run_in "$ROOT_DIR/Feeder" "feeder_stage1" ./start.sh IntersectWith "$STEPWISE_INPUT_FILE" "$PROXY_XSUB"
	if [[ -n "${LOGGER_PID:-}" ]]; then
		wait_for_pid_with_timeout "$LOGGER_PID" "$STAGE1_TIMEOUT" || true
	fi
	cleanup_stage || true
else
	echo "[stepwise] RESUME: Skipping Stage 1; using existing IntersectResult file"
fi

## Stage 2: IntersectResults; capture Shader
if [[ "$RESUME_STAGE" -le 2 ]]; then
	echo "[stepwise] Stage 2: IntersectResults -> Shader"
	STAGE1_OUT_FILE="$RUN_DIR/oIntersectResult.txt"
	if [[ "$RESUME_STAGE" -ge 2 && -n "$RESUME_STAGE1_FILE" ]]; then
		STAGE1_OUT_FILE="$RESUME_STAGE1_FILE"
	fi
	# If the provided file already represents IntersectResults aggregate output (oIntersectResults.txt),
	# bypass Stage 2 and let Shader consume it directly as Stage 2 output.
	if [[ "$(basename "$STAGE1_OUT_FILE")" == "oIntersectResults.txt" ]]; then
		echo "[stepwise] BYPASS: Detected aggregate file ($STAGE1_OUT_FILE); skipping Stage 2 and using it as Shader input."
		export RESUME_STAGE2_FILE="$STAGE1_OUT_FILE"
	else
		run_in "$ROOT_DIR/IntersectResults" intersect_results ./start.sh
		export LOG_BASEDIR="$RUN_DIR"
		run_in "$ROOT_DIR/Logger" "logger_stage2" ./start.sh "$BUS_SHADER" "$TOPIC_SHADER" Shader.txt
		sleep 0.15
		LOGGER_PID=$LAST_PID
		run_in "$ROOT_DIR/Feeder" "feeder_stage2" ./start.sh "$TOPIC_INTERSECT_RESULT" "$STAGE1_OUT_FILE" "$BUS_INTERSECT_RESULT"
		if [[ -n "${LOGGER_PID:-}" ]]; then
			wait_for_pid_with_timeout "$LOGGER_PID" "$STAGE2_TIMEOUT" || true
		fi
		cleanup_stage || true
	fi
else
	echo "[stepwise] RESUME: Skipping Stage 2; using existing Shader input for later stages"
fi

## Stage 3: Shader; capture COLOR
if [[ "$RESUME_STAGE" -le 3 ]]; then
	echo "[stepwise] Stage 3: Shader -> COLOR"
	# Allow using a reference Stage 2 output file when resuming
	STAGE2_OUT_FILE_DEFAULT="$RUN_DIR/oShader.txt"
	STAGE2_OUT_FILE_RESUME=${RESUME_STAGE2_FILE:-}
	if [[ -n "$STAGE2_OUT_FILE_RESUME" ]]; then
		STAGE2_OUT_FILE="$STAGE2_OUT_FILE_RESUME"
	else
		STAGE2_OUT_FILE="$STAGE2_OUT_FILE_DEFAULT"
	fi

	if has_data "$STAGE2_OUT_FILE"; then
		# Normal path: drive Shader and capture COLOR
		# Bind the Shader subscriber so Feeder can connect in stepwise
	# Bind Shader SUB so Feeder can connect on BUS_SHADER
	export SHADER_BIND_SUB="${SHADER_BIND_SUB:-${SHADER_BIND_SUB_DEFAULT:-1}}"
		run_in "$ROOT_DIR/Shader" shader ./start.sh
		export LOG_BASEDIR="$RUN_DIR"
		run_in "$ROOT_DIR/Logger" "logger_stage3" ./start.sh "$BUS_COLOR" "$TOPIC_COLOR" COLOR.txt
		sleep 0.15
		LOGGER_PID=$LAST_PID
		run_in "$ROOT_DIR/Feeder" "feeder_stage3" ./start.sh "$TOPIC_SHADER" "$STAGE2_OUT_FILE" "$BUS_SHADER"
		if [[ -n "${LOGGER_PID:-}" ]]; then
			wait_for_pid_with_timeout "$LOGGER_PID" "$STAGE3_TIMEOUT" || true
		fi
		cleanup_stage || true
	else
		# Fallback: no Shader input; publish EOF directly to COLOR bus so logger completes
		export LOG_BASEDIR="$RUN_DIR"
		run_in "$ROOT_DIR/Logger" "logger_stage3" ./start.sh "$BUS_COLOR" "$TOPIC_COLOR" COLOR.txt
		sleep 0.1
		run_in "$ROOT_DIR/Feeder" "feeder_stage3_eof" ./start.sh "$TOPIC_COLOR" "$STAGE2_OUT_FILE" "$BUS_COLOR"
		wait_and_clear_pids
	fi
else
	echo "[stepwise] RESUME: Skipping Stage 3; using existing COLOR input for later stages"
fi

# Stage 4: ColorResults; capture DEPTH
if [[ "$RESUME_STAGE" -le 4 ]]; then
echo "[stepwise] Stage 4: ColorResults -> DEPTH"
STAGE3_OUT_FILE_DEFAULT="$RUN_DIR/oCOLOR.txt"
STAGE3_OUT_FILE_RESUME=${RESUME_STAGE3_FILE:-}
if [[ -n "$STAGE3_OUT_FILE_RESUME" ]]; then
	STAGE3_OUT_FILE="$STAGE3_OUT_FILE_RESUME"
else
	STAGE3_OUT_FILE="$STAGE3_OUT_FILE_DEFAULT"
fi
if has_data "$STAGE3_OUT_FILE"; then
	# Sanitize Stage 3 artifact to avoid NaN/Inf or extreme magnitudes upsetting parsers
	SANITIZED_STAGE3="$RUN_DIR/oCOLOR.sanitized.txt"
	python3 "$ROOT_DIR/scripts/tools/sanitize_artifact.py" "$STAGE3_OUT_FILE" "$SANITIZED_STAGE3" >/dev/null 2>&1 || cp "$STAGE3_OUT_FILE" "$SANITIZED_STAGE3"
	# Bind ColorResults SUB so Feeder can connect on BUS_COLOR
	export COLORRESULTS_BIND_SUB="${COLORRESULTS_BIND_SUB:-${COLORRESULTS_BIND_SUB_DEFAULT:-1}}"
	run_in "$ROOT_DIR/ColorResults" colorresults ./start.sh
	export LOG_BASEDIR="$RUN_DIR"
	run_in "$ROOT_DIR/Logger" "logger_stage4" ./start.sh "$BUS_DEPTH" "$TOPIC_DEPTH" DEPTH.txt
	sleep 0.15
	LOGGER_PID=$LAST_PID
	run_in "$ROOT_DIR/Feeder" "feeder_stage4" ./start.sh "$TOPIC_COLOR" "$SANITIZED_STAGE3" "$BUS_COLOR"
	STAGE4_TIMEOUT=${STAGE4_TIMEOUT:-$STAGE_DEFAULT_TIMEOUT}
	if [[ -n "${LOGGER_PID:-}" ]]; then
		wait_for_pid_with_timeout "$LOGGER_PID" "$STAGE4_TIMEOUT" || true
	fi
	cleanup_stage || true
else
	export LOG_BASEDIR="$RUN_DIR"
	run_in "$ROOT_DIR/Logger" "logger_stage4" ./start.sh "$BUS_DEPTH" "$TOPIC_DEPTH" DEPTH.txt
	sleep 0.1
	run_in "$ROOT_DIR/Feeder" "feeder_stage4_eof" ./start.sh "$TOPIC_DEPTH" "$STAGE3_OUT_FILE" "$BUS_DEPTH"
	wait_and_clear_pids
fi
else
	echo "[stepwise] RESUME: Skipping Stage 4"
fi

# Stage 5: DepthChart; capture PNG channel
if [[ "$RESUME_STAGE" -le 5 ]]; then
echo "[stepwise] Stage 5: DepthChart -> PNG"
STAGE4_OUT_FILE_DEFAULT="$RUN_DIR/oDEPTH.txt"
STAGE4_OUT_FILE_RESUME=${RESUME_STAGE4_FILE:-}
if [[ -n "$STAGE4_OUT_FILE_RESUME" ]]; then
	STAGE4_OUT_FILE="$STAGE4_OUT_FILE_RESUME"
else
	STAGE4_OUT_FILE="$STAGE4_OUT_FILE_DEFAULT"
fi
if has_data "$STAGE4_OUT_FILE"; then
	# Bind DepthChart SUB so Feeder can connect on BUS_DEPTH
	export DEPTHCHART_BIND_SUB="${DEPTHCHART_BIND_SUB:-${DEPTHCHART_BIND_SUB_DEFAULT:-1}}"
	run_in "$ROOT_DIR/DepthChart" depthchart ./start.sh
	export LOG_BASEDIR="$RUN_DIR"
	run_in "$ROOT_DIR/Logger" "logger_stage5" ./start.sh "$BUS_PNG" "$TOPIC_PNG" PNG.txt
	sleep 0.15
	LOGGER_PID=$LAST_PID
	run_in "$ROOT_DIR/Feeder" "feeder_stage5" ./start.sh "$TOPIC_DEPTH" "$STAGE4_OUT_FILE" "$BUS_DEPTH"
	STAGE5_TIMEOUT=${STAGE5_TIMEOUT:-$STAGE_DEFAULT_TIMEOUT}
	if [[ -n "${LOGGER_PID:-}" ]]; then
		wait_for_pid_with_timeout "$LOGGER_PID" "$STAGE5_TIMEOUT" || true
	fi
	cleanup_stage || true
else
	export LOG_BASEDIR="$RUN_DIR"
	run_in "$ROOT_DIR/Logger" "logger_stage5" ./start.sh "$BUS_PNG" "$TOPIC_PNG" PNG.txt
	sleep 0.1
	run_in "$ROOT_DIR/Feeder" "feeder_stage5_eof" ./start.sh "$TOPIC_PNG" "$STAGE4_OUT_FILE" "$BUS_PNG"
	wait_and_clear_pids
fi
else
	echo "[stepwise] RESUME: Skipping Stage 5"
fi

# Stage 6: Writer; produce final PNG from logged PNG channel
echo "[stepwise] Stage 6: Writer (sink)"
STAGE5_OUT_FILE_DEFAULT="$RUN_DIR/oPNG.txt"
STAGE5_OUT_FILE_RESUME=${RESUME_STAGE5_FILE:-}
if [[ -n "$STAGE5_OUT_FILE_RESUME" ]]; then
	STAGE5_OUT_FILE="$STAGE5_OUT_FILE_RESUME"
else
	STAGE5_OUT_FILE="$STAGE5_OUT_FILE_DEFAULT"
fi
# Bind Writer's subscriber directly (simpler and correct for two-party PUB/SUB)
export WRITER_BIND_SUB=1
run_in "$ROOT_DIR/Writer" writer ./start.sh
sleep 0.15
run_in "$ROOT_DIR/Feeder" "feeder_stage6" ./start.sh "$TOPIC_PNG" "$STAGE5_OUT_FILE" "$BUS_PNG"

# Wait for PNG output
TIMEOUT=${SMOKE_WAIT_SECONDS:-60}
echo "[stepwise] Waiting up to ${TIMEOUT}s for $PNG_FILE ..."
for ((i=0; i< TIMEOUT; i++)); do
	if [ -f "$PNG_FILE" ] && [ -s "$PNG_FILE" ]; then
		echo "[stepwise] PASS at t=${i}s: $PNG_FILE exists and is non-empty"
	cleanup_all || true
	exit 0
	fi
	sleep 1
done

echo "[stepwise] FAIL: $PNG_FILE missing or empty after ${TIMEOUT}s"
cleanup_all || true
exit 1
