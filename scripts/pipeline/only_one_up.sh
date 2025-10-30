#!/usr/bin/env bash
# Run exactly one pipeline stage in isolation, wiring only what it needs.
# Examples:
#   bash scripts/pipeline/only_one_up.sh 4               # Stage 4 using runs/complete/oCOLOR.txt
#   STAGE=4 INPUT_FILE=runs/complete/oCOLOR.txt bash scripts/pipeline/only_one_up.sh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
RUN_ID="${RUN_ID:-one-up-$(date +%Y%m%d-%H%M%S)}"
RUN_DIR="$ROOT_DIR/runs/$RUN_ID"
mkdir -p "$RUN_DIR"
echo "[one-up] Run directory: $RUN_DIR"

source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"
export SMOKE_MODE=${SMOKE_MODE:-1}

STAGE="${1:-${STAGE:-4}}"

global_pids=()
stage_pids=()
cleanup() {
	for pid in "${stage_pids[@]:-}"; do kill "$pid" 2>/dev/null || true; done
	sleep 0.2
	for pid in "${stage_pids[@]:-}"; do kill -9 "$pid" 2>/dev/null || true; done
}
trap cleanup EXIT INT TERM

launch() {
	local name=$1; shift
	echo "[one-up] starting $name"
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

case "$STAGE" in
	4)
		echo "[one-up] Stage 4: ColorResults -> DEPTH"
		# Inputs
		INPUT_FILE="${INPUT_FILE:-$ROOT_DIR/runs/complete/oCOLOR.txt}"
		# Normalize to absolute path if a relative path was provided
		if [[ "$INPUT_FILE" != /* ]]; then
			INPUT_FILE="$ROOT_DIR/${INPUT_FILE#./}"
		fi
		if [[ ! -s "$INPUT_FILE" ]]; then
			echo "[one-up] ERROR: Missing COLOR input file: $INPUT_FILE" >&2
			exit 2
		fi

		# Launch stage components
	export COLORRESULTS_BIND_SUB=1
	run_in "$ROOT_DIR/ColorResults" colorresults ./start.sh
	export LOG_BASEDIR="$RUN_DIR"
	export LOGGER_BIND=1
	run_in "$ROOT_DIR/Logger" logger_stage4 ./start.sh "$BUS_DEPTH" "$TOPIC_DEPTH" DEPTH.txt
		LOGGER_PID=$LAST_PID
		sleep 0.15
		export FEEDER_LIMIT=${FEEDER_LIMIT:--1}
		run_in "$ROOT_DIR/Feeder" feeder_stage4 ./start.sh "$TOPIC_COLOR" "$INPUT_FILE" "$BUS_COLOR"

	# Wait for output file to appear and be non-empty (Logger writes oDEPTH.txt)
	OUT_FILE="$RUN_DIR/oDEPTH.txt"
		LOGGER_TIMEOUT=${LOGGER_TIMEOUT:-900}
		echo "[one-up] Waiting up to ${LOGGER_TIMEOUT}s for $OUT_FILE ..."
		for ((i=0;i<LOGGER_TIMEOUT;i++)); do
			if [[ -s "$OUT_FILE" ]]; then
				echo "[one-up] DEPTH output ready at t=${i}s"
				break
			fi
			sleep 1
		done
		;;
	5)
		echo "[one-up] Stage 5: DepthChart -> PNG"
		INPUT_FILE="${INPUT_FILE:-$ROOT_DIR/runs/complete/oDEPTH.txt}"
		# Normalize to absolute path if a relative path was provided
		if [[ "$INPUT_FILE" != /* ]]; then
			INPUT_FILE="$ROOT_DIR/${INPUT_FILE#./}"
		fi
		if [[ ! -s "$INPUT_FILE" ]]; then
			echo "[one-up] ERROR: Missing DEPTH input file: $INPUT_FILE" >&2
			exit 2
		fi

		# Ensure fresh PNG channel capture
		export LOG_BASEDIR="$RUN_DIR"
		export LOGGER_BIND=1
		export DEPTHCHART_BIND_SUB=1
		run_in "$ROOT_DIR/DepthChart" depthchart ./start.sh
		run_in "$ROOT_DIR/Logger" logger_stage5 ./start.sh "$BUS_PNG" "$TOPIC_PNG" PNG.txt
		sleep 0.15
		export FEEDER_LIMIT=${FEEDER_LIMIT:--1}
		run_in "$ROOT_DIR/Feeder" feeder_stage5 ./start.sh "$TOPIC_DEPTH" "$INPUT_FILE" "$BUS_DEPTH"

		# Wait for PNG log to be created
		OUT_FILE="$RUN_DIR/oPNG.txt"
		LOGGER_TIMEOUT=${LOGGER_TIMEOUT:-900}
		echo "[one-up] Waiting up to ${LOGGER_TIMEOUT}s for $OUT_FILE ..."
		for ((i=0;i<LOGGER_TIMEOUT;i++)); do
			if [[ -s "$OUT_FILE" ]]; then
				echo "[one-up] PNG channel output ready at t=${i}s"
				break
			fi
			sleep 1
		done
		;;
	6)
		echo "[one-up] Stage 6: Writer -> PNG file"
		INPUT_FILE="${INPUT_FILE:-$ROOT_DIR/runs/complete/oPNG.txt}"
		# Normalize to absolute path if a relative path was provided
		if [[ "$INPUT_FILE" != /* ]]; then
			INPUT_FILE="$ROOT_DIR/${INPUT_FILE#./}"
		fi
		if [[ ! -s "$INPUT_FILE" ]]; then
			echo "[one-up] ERROR: Missing PNG channel input file: $INPUT_FILE" >&2
			exit 2
		fi

		# Launch Writer binding its SUB so Feeder can connect directly
	export WRITER_BIND_SUB=1
	OUTPUT_PNG="$RUN_DIR/test.png"
	run_in "$ROOT_DIR/Writer" writer_stage6 ./start.sh "$OUTPUT_PNG"
	WRITER_PID=$LAST_PID
		sleep 0.15
		# Drive the PNG channel from the precomputed log
		export FEEDER_LIMIT=${FEEDER_LIMIT:--1}
		run_in "$ROOT_DIR/Feeder" feeder_stage6 ./start.sh "$TOPIC_PNG" "$INPUT_FILE" "$BUS_PNG"

		# Wait for output PNG to be created and non-empty
		OUT_FILE="$OUTPUT_PNG"
		WRITER_TIMEOUT=${WRITER_TIMEOUT:-900}
		echo "[one-up] Waiting up to ${WRITER_TIMEOUT}s for $OUT_FILE ..."
		for ((i=0;i<WRITER_TIMEOUT;i++)); do
			if [[ -s "$OUT_FILE" ]]; then
				echo "[one-up] Final PNG ready at t=${i}s: $OUT_FILE"
				break
			fi
			sleep 1
		done
		# Give Writer time to receive EOF and print [STATS], up to 60s, or until the process exits
		if [[ -n "${WRITER_PID:-}" ]]; then
			echo "[one-up] Waiting for Writer to finish (up to 60s) ..."
			for ((i=0;i<60;i++)); do
				if ! kill -0 "$WRITER_PID" 2>/dev/null; then
					echo "[one-up] Writer exited after ${i}s"
					break
				fi
				sleep 1
			done
		fi
		;;
	6)
		echo "[one-up] Stage 6: Writer <- PNG"
		INPUT_FILE="${INPUT_FILE:-$ROOT_DIR/runs/complete/oPNG.txt}"
		# Normalize to absolute path if a relative path was provided
		if [[ "$INPUT_FILE" != /* ]]; then
			INPUT_FILE="$ROOT_DIR/${INPUT_FILE#./}"
		fi
		if [[ ! -s "$INPUT_FILE" ]]; then
			echo "[one-up] ERROR: Missing PNG channel input file: $INPUT_FILE" >&2
			exit 2
		fi

		# Start a binder on the PNG bus using Logger (so PUB can connect)
		export LOG_BASEDIR="$RUN_DIR"
		export LOGGER_BIND=1
		run_in "$ROOT_DIR/Logger" logger_stage6 ./start.sh "$BUS_PNG" "$TOPIC_PNG" PNG.txt
		sleep 0.15

		# Launch Writer; override output path to this run directory
		echo "[one-up] starting writer"
		"$ROOT_DIR/bin/Writer" Writer "$TOPIC_PNG" "$BUS_PNG" dummy dummy "$RUN_DIR/test.png" >"$RUN_DIR/writer_stage6.log" 2>&1 &
		LAST_PID=$!
		stage_pids+=("$LAST_PID")

		# Feed the PNG channel from the provided oPNG.txt
		export FEEDER_LIMIT=${FEEDER_LIMIT:--1}
		run_in "$ROOT_DIR/Feeder" feeder_stage6 ./start.sh "$TOPIC_PNG" "$INPUT_FILE" "$BUS_PNG"

		# Wait for Writer to produce the PNG
		OUT_PNG="$RUN_DIR/test.png"
		WRITER_TIMEOUT=${WRITER_TIMEOUT:-900}
		echo "[one-up] Waiting up to ${WRITER_TIMEOUT}s for $OUT_PNG ..."
		for ((i=0;i<WRITER_TIMEOUT;i++)); do
			if [[ -s "$OUT_PNG" ]]; then
				echo "[one-up] PNG file ready at t=${i}s -> $OUT_PNG"
				break
			fi
			sleep 1
		done
		;;
	*)
		echo "[one-up] ERROR: Stage $STAGE not implemented yet"
		exit 1
		;;
esac

echo "[one-up] Artifacts in: $RUN_DIR"
if [[ "$STAGE" == "4" ]]; then
	echo "[one-up] Tail of logger output:"
	tail -n 100 "$RUN_DIR"/logger_stage4.log || true
	echo "[one-up] Tail of ColorResults output:"
	tail -n 100 "$RUN_DIR"/colorresults.log || true
	echo "[one-up] Tail of Feeder output:"
	tail -n 60 "$RUN_DIR"/feeder_stage4.log || true
elif [[ "$STAGE" == "5" ]]; then
	echo "[one-up] Tail of DepthChart output:"
	tail -n 100 "$RUN_DIR"/depthchart.log || true
	echo "[one-up] Tail of logger (PNG) output:"
	tail -n 100 "$RUN_DIR"/logger_stage5.log || true
	echo "[one-up] Tail of Feeder (DEPTH) output:"
	tail -n 60 "$RUN_DIR"/feeder_stage5.log || true
fi
