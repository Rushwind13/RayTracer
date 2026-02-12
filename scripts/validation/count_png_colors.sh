#!/usr/bin/env bash
# Count approximate unique colors for a given Logger-formatted artifact by rendering via Writer
# We report: distinct_colors_from_hits + 1 (for the miss color used in Writer's BRUTE_FORCE path)
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

if [[ $# -lt 1 ]]; then
	echo "Usage: $0 <artifact.txt>" >&2
	exit 2
fi
ART="$1"
if [[ ! -s "$ART" ]]; then
	echo "ERROR: Missing or empty artifact: $ART" >&2
	exit 2
fi

TMP_OPNG="${TMPDIR:-/tmp}/oPNG.$$.txt"
TMP_LOG="${TMPDIR:-/tmp}/writer.$$.log"
OUT_PNG="${TMPDIR:-/tmp}/count_colors.$$.png"

python3 "$ROOT_DIR/scripts/tools/to_opng.py" "$TMP_OPNG" "$ART" >/dev/null

export DYLD_LIBRARY_PATH="$ROOT_DIR/../zmq_widgets/bin:${DYLD_LIBRARY_PATH:-}"

# Use fixed PNG bus from central config so diagnostics match pipeline ports
PNG_SOCKET="$BUS_PNG"

# Launch Writer bound on temp PNG bus and capture output
(
	cd "$ROOT_DIR/Writer" && \
	WRITER_BIND_SUB=1 WRITER_INPUT_SOCKET="$PNG_SOCKET" ./start.sh "$OUT_PNG" > "$TMP_LOG" 2>&1 & echo $! > "$TMP_OPNG.pid"
)
WR_PID=$(cat "$TMP_OPNG.pid")
sleep 0.2

# Feed oPNG to PNG bus
(
	cd "$ROOT_DIR/Feeder" && \
	./start.sh "$TOPIC_PNG" "$TMP_OPNG" "$PNG_SOCKET" > /dev/null 2>&1 || true
)

# Wait for Writer to finish on its own after receiving EOF (up to 15s)
for _ in $(seq 1 150); do
	if ! ps -p "$WR_PID" >/dev/null 2>&1; then break; fi
	sleep 0.1
done
# If still running, try graceful kill
if ps -p "$WR_PID" >/dev/null 2>&1; then kill "$WR_PID" >/dev/null 2>&1 || true; fi

# Parse distinct_colors from Writer stats; fall back to 0 when not present
HIT_COLORS=$(grep -Eo "\[STATS\] distinct_colors: [0-9]+" "$TMP_LOG" | awk '{print $4}' | tail -n1 || true)
if [[ -z "${HIT_COLORS:-}" ]]; then HIT_COLORS=0; fi
TOTAL=$(( HIT_COLORS + 1 ))
if [[ "$TOTAL" -le 1 ]]; then
	# Fallback: approximate by scanning artifact for any hit (short lines with 3rd field >= 0)
	if awk -F, 'NR%2==0{next} {next} END{}' /dev/null >/dev/null 2>&1; then :; fi # noop to ensure awk exists
	if awk -F, 'NR%2==0 { if ($3+0 >= 0) { print "HIT"; exit } } END{ if (!NR) exit 0; else print "MISS" }' "$ART" | grep -q "HIT"; then
		echo 2
	else
		echo 1
	fi
else
	echo "$TOTAL"
fi

# If debugging, keep artifacts and show quick tail of Writer log
if [[ "${COUNT_COLORS_DEBUG:-0}" != "0" ]]; then
	echo "[debug] writer log: $TMP_LOG" >&2
	tail -n 25 "$TMP_LOG" >&2 || true
else
	rm -f "$TMP_OPNG" "$TMP_OPNG.pid" "$TMP_LOG" "$OUT_PNG"
fi
