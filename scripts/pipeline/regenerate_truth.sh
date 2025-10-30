#!/usr/bin/env bash
# Regenerate a full, deterministic set of stepwise rendering artifacts from data/pixels.txt
# Captures: oIntersectResult, oShader, oBKG, oBlack, oLit, oCOLOR, oDEPTH, oPNG, final PNG
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"
export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"
RUN_ID="truth-$(date +%Y%m%d-%H%M%S)"
RUN_DIR="$ROOT_DIR/runs/$RUN_ID"
mkdir -p "$RUN_DIR"
echo "[truth] Run directory: $RUN_DIR"

launch() { local name=$1; shift; echo "[truth] starting $name"; "$@" >"$RUN_DIR/${name}.log" 2>&1 & LAST_PID=$!; pids+=("$LAST_PID"); }
run_in() { local dir=$1; shift; pushd "$dir" >/dev/null; launch "$@"; popd >/dev/null; }
cleanup_stage(){ for pid in "${pids[@]:-}"; do kill "$pid" 2>/dev/null || true; done; sleep 0.2; for pid in "${pids[@]:-}"; do kill -9 "$pid" 2>/dev/null || true; done; pids=(); }

pids=()
# Start proxy for IntersectWith fan-out
"$ROOT_DIR/../zmq_widgets/bin/ControlChannel" "$PROXY_XSUB" "$PROXY_XPUB" >"$RUN_DIR/proxy.log" 2>&1 & proxy_pid=$!

# Stage 1: IntersectWith (many) -> IntersectResults (via proxy)
for OBJ in "${OBJECTS[@]}"; do run_in "$ROOT_DIR/IntersectWith" "intersect_with_${OBJ}" ./start.sh "$OBJ" "$PROXY_XSUB"; done
# Capture IntersectResult via logger on proxy XPUB
run_in "$ROOT_DIR/Logger" logger_stage1 ./start.sh "$PROXY_XPUB" "$TOPIC_INTERSECT_RESULT" IntersectResult.txt
sleep 0.15; LOGGER1=$LAST_PID
# Feed pixels
STEPWISE_INPUT_FILE="$ROOT_DIR/data/pixels.txt"
run_in "$ROOT_DIR/Feeder" feeder_stage1 ./start.sh IntersectWith "$STEPWISE_INPUT_FILE" "$PROXY_XSUB"
wait "$LOGGER1" || true; cleanup_stage || true

# Stage 2: IntersectResults -> log Shader, BKG, BLACK, LIT
run_in "$ROOT_DIR/IntersectResults" intersect_results ./start.sh
run_in "$ROOT_DIR/Logger" logger_shader ./start.sh "$BUS_SHADER" Shader.txt
run_in "$ROOT_DIR/Logger" logger_bkg ./start.sh "$BUS_SHADER" BKG.txt
run_in "$ROOT_DIR/Logger" logger_black ./start.sh "$BUS_SHADER" BLACK.txt
run_in "$ROOT_DIR/Logger" logger_lit ./start.sh "$BUS_SHADER" LIT.txt
sleep 0.15; LOGGER_LAST=$LAST_PID
run_in "$ROOT_DIR/Feeder" feeder_stage2 ./start.sh "$TOPIC_INTERSECT_RESULT" "$RUN_DIR/oIntersectResult.txt" "$BUS_INTERSECT_RESULT"
wait "$LOGGER_LAST" || true; cleanup_stage || true

# Stage 3: Shader + Background + Black + Lit -> COLOR
export SHADER_BIND_SUB=1
export BACKGROUND_BIND_SUB=1
export BLACK_BIND_SUB=1
export LIT_BIND_SUB=1
run_in "$ROOT_DIR/Shader" shader ./start.sh
run_in "$ROOT_DIR/Background" background ./start.sh
run_in "$ROOT_DIR/Black" black ./start.sh
run_in "$ROOT_DIR/Lit" lit ./start.sh
run_in "$ROOT_DIR/Logger" logger_stage3 ./start.sh "$BUS_COLOR" "$TOPIC_COLOR" COLOR.txt
sleep 0.2
run_in "$ROOT_DIR/Feeder" feeder_shader ./start.sh "$TOPIC_SHADER" "$RUN_DIR/oShader.txt" "$BUS_SHADER"
run_in "$ROOT_DIR/Feeder" feeder_bkg ./start.sh BKG "$RUN_DIR/oBKG.txt" "$BUS_SHADER"
run_in "$ROOT_DIR/Feeder" feeder_black ./start.sh BLACK "$RUN_DIR/oBLACK.txt" "$BUS_SHADER"
run_in "$ROOT_DIR/Feeder" feeder_lit ./start.sh LIT "$RUN_DIR/oLIT.txt" "$BUS_SHADER"
# Wait for COLOR logger to finish
wait "$LAST_PID" || true; cleanup_stage || true

# Stage 4: ColorResults -> DEPTH
run_in "$ROOT_DIR/ColorResults" colorresults ./start.sh
run_in "$ROOT_DIR/Logger" logger_stage4 ./start.sh "$BUS_DEPTH" "$TOPIC_DEPTH" DEPTH.txt
sleep 0.15; LOGGER_LAST=$LAST_PID
run_in "$ROOT_DIR/Feeder" feeder_stage4 ./start.sh "$TOPIC_COLOR" "$RUN_DIR/oCOLOR.txt" "$BUS_COLOR"
wait "$LOGGER_LAST" || true; cleanup_stage || true

# Stage 5: DepthChart -> PNG
run_in "$ROOT_DIR/DepthChart" depthchart ./start.sh
run_in "$ROOT_DIR/Logger" logger_stage5 ./start.sh "$BUS_PNG" "$TOPIC_PNG" PNG.txt
sleep 0.15; LOGGER_LAST=$LAST_PID
run_in "$ROOT_DIR/Feeder" feeder_stage5 ./start.sh "$TOPIC_DEPTH" "$RUN_DIR/oDEPTH.txt" "$BUS_DEPTH"
wait "$LOGGER_LAST" || true; cleanup_stage || true

# Stage 6: Writer -> final PNG
export WRITER_BIND_SUB=1
run_in "$ROOT_DIR/Writer" writer ./start.sh "$RUN_DIR/test.png"
sleep 0.15
run_in "$ROOT_DIR/Feeder" feeder_stage6 ./start.sh "$TOPIC_PNG" "$RUN_DIR/oPNG.txt" "$BUS_PNG"
# Wait briefly for file
for i in {1..60}; do [[ -s "$RUN_DIR/test.png" ]] && break; sleep 1; done

kill "$proxy_pid" 2>/dev/null || true
sleep 0.2; kill -9 "$proxy_pid" 2>/dev/null || true

# Summarize
wc -l "$RUN_DIR"/o* 2>/dev/null || true
