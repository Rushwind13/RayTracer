#!/usr/bin/env bash
# Launch the distributed ray tracer pipeline with a minimal bus proxy and a short feeder-driven smoke test.
# MacOS bash-compatible.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
BIN_DIR="$ROOT_DIR/bin"
LOG_DIR="$ROOT_DIR/runlogs"
mkdir -p "$LOG_DIR"

# Ensure zmq_widgets dynamic library is discoverable on macOS
export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"

pids=()
cleanup() {
  echo "\n[all_up] Cleaning up..."
  # First try TERM
  for pid in "${pids[@]:-}"; do
    if kill -0 "$pid" 2>/dev/null; then
      kill "$pid" 2>/dev/null || true
    fi
  done
  # Short grace period
  sleep 0.2
  # Escalate to KILL for any remaining
  for pid in "${pids[@]:-}"; do
    if kill -0 "$pid" 2>/dev/null; then
      kill -9 "$pid" 2>/dev/null || true
    fi
  done
}
trap cleanup EXIT INT TERM

launch() {
  local name=$1; shift
  local log="$LOG_DIR/${name}.log"
  echo "[all_up] starting $name -> $log"
  "$@" >"$log" 2>&1 &
  pids+=("$!")
}

# Run a launch inside a directory without using a subshell, so we keep PIDs
run_in() {
  local dir=$1; shift
  pushd "$dir" >/dev/null
  launch "$@"
  popd >/dev/null
}

# 1) Start XPUB/XSUB proxy for INTERSECT bus
launch proxy "$ROOT_DIR/../zmq_widgets/bin/ControlChannel"

# Give the proxy a moment to bind ports
sleep 0.2

# 2) Start sink and aggregators in order
# Writer listens on PNG (1308)
run_in "$ROOT_DIR/Writer" writer ./start.sh
# DepthChart listens on DEPTH (1307)
run_in "$ROOT_DIR/DepthChart" depthchart ./start.sh
# ColorResults listens on COLOR (1303)
run_in "$ROOT_DIR/ColorResults" colorresults ./start.sh

# 3) Background/Black/Lit receive routed shading events
run_in "$ROOT_DIR/Background" background ./start.sh
run_in "$ROOT_DIR/Black" black ./start.sh
run_in "$ROOT_DIR/Lit" lit ./start.sh

# 4) Shader consumes SHADE and emits shadow tests to INTERSECT bus
run_in "$ROOT_DIR/Shader" shader ./start.sh

# 5) IntersectResults aggregates per-pixel object results
run_in "$ROOT_DIR/IntersectResults" intersect_results ./start.sh

# 6) Start IntersectWith instances for each world object (fan-out workers)
# Keep this list in sync with bin/World.json objects
OBJECTS=(
  box1
  sphere2
  sphere3
  plane_floor
  plane_l_wall
  plane_r_wall
)
for OBJ in "${OBJECTS[@]}"; do
  run_in "$ROOT_DIR/IntersectWith" "intersect_with_${OBJ}" ./start.sh "$OBJ"
done

# 7) Feed a limited number of pixels into the INTERSECT bus
#    Uses data/pixels.txt by default; limit to keep smoke quick
export FEEDER_LIMIT=${FEEDER_LIMIT:-15000}
run_in "$ROOT_DIR/Feeder" feeder ./start.sh IntersectWith "$ROOT_DIR/data/pixels.txt"

# 8) Wait for output PNG to appear (poll up to timeout), exit early when ready
PNG_FILE="$ROOT_DIR/bin/test.png"
TIMEOUT=${SMOKE_WAIT_SECONDS:-120}
echo "[all_up] Waiting up to ${TIMEOUT}s for $PNG_FILE ..."
for ((i=0; i< TIMEOUT; i++)); do
  if [ -f "$PNG_FILE" ] && [ -s "$PNG_FILE" ]; then
    echo "[all_up] Smoke test PASS at t=${i}s: $PNG_FILE exists and is non-empty"
    # Clean up started processes before exiting when in smoke mode
    if [ -n "${SMOKE_MODE-}" ] && [ "${SMOKE_MODE}" != "0" ]; then
      cleanup || true
      # As a safeguard, run the global teardown to catch any stragglers
      "$ROOT_DIR/scripts/all_down.sh" || true
    fi
    exit 0
  fi
  sleep 1
done

echo "[all_up] Smoke test FAIL: $PNG_FILE missing or empty after ${TIMEOUT}s"
# Ensure cleanup is attempted even on failure when in smoke mode
if [ -n "${SMOKE_MODE-}" ] && [ "${SMOKE_MODE}" != "0" ]; then
  cleanup || true
  "$ROOT_DIR/scripts/all_down.sh" || true
fi
exit 1
