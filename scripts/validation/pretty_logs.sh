#!/usr/bin/env bash
# Pretty-print a RayTracer run directory: artifacts and log tails.
# Usage:
#   scripts/validation/pretty_logs.sh [RUN_DIR | latest | latest-one-up | latest-stepwise] [-n LINES]
# Defaults: select the latest directory under runs/, tail last 80 lines.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
RUNS_DIR="$ROOT_DIR/runs"
SEL="${1:-latest}"
shift || true
LINES=80
while [[ "${1:-}" =~ ^- ]]; do
	case "$1" in
		-n|--lines)
			LINES="${2:-80}"; shift 2;;
		--) shift; break;;
		*) echo "Unknown option: $1"; exit 2;;
	esac
done

pick_latest() {
	local pattern="$1"
	ls -1dt $pattern 2>/dev/null | head -n1 | sed 's#//#/#g'
}

case "$SEL" in
	latest)
		RUN_DIR=$(pick_latest "$RUNS_DIR/*")
		;;
	latest-one-up)
		RUN_DIR=$(pick_latest "$RUNS_DIR/one-up-*")
		;;
	latest-stepwise)
		RUN_DIR=$(pick_latest "$RUNS_DIR/20*")
		;;
	*)
		RUN_DIR="$SEL"
		;;
esac

if [[ -z "${RUN_DIR:-}" || ! -d "$RUN_DIR" ]]; then
	echo "[pretty] ERROR: Run directory not found: $RUN_DIR" >&2
	exit 2
fi

hr() { printf '\n%s\n' "============================================================"; }
sec() { hr; echo "$1"; hr; }

echo "[pretty] Run directory: $RUN_DIR"

sec "files"
ls -lh "$RUN_DIR"

# Key outputs
PNG_CANDIDATES=("$RUN_DIR/test.png" "$ROOT_DIR/bin/test.png")
for png in "${PNG_CANDIDATES[@]}"; do
	if [[ -f "$png" ]]; then
		echo "[pretty] PNG: $png ($(du -h "$png" | awk '{print $1}'))"
	fi
done
for f in "$RUN_DIR"/o*.txt; do
	if [[ -f "$f" ]]; then
		echo "[pretty] OUT: $(basename "$f") size=$(stat -f%z "$f" 2>/dev/null || stat -c%s "$f" 2>/dev/null)"
	fi
done

# Helpful tails
tail_if() {
	local f=$1; shift
	if [[ -f "$f" ]]; then
		sec "$(basename "$f") (last $LINES)"
		tail -n "$LINES" "$f"
	fi
}

# Prefer stage-specific logs when present
tail_if "$RUN_DIR/writer.log"
tail_if "$RUN_DIR/writer_stage6.log"
tail_if "$RUN_DIR/logger_stage5.log"
tail_if "$RUN_DIR/depthchart.log"
tail_if "$RUN_DIR/logger_stage4.log"
tail_if "$RUN_DIR/colorresults.log"
tail_if "$RUN_DIR/logger_stage3.log"
tail_if "$RUN_DIR/shader.log"
tail_if "$RUN_DIR/logger_stage2.log"
tail_if "$RUN_DIR/intersect_results.log"
tail_if "$RUN_DIR/logger_stage1.log"
tail_if "$RUN_DIR/proxy.log"

# Any remaining .log files
others=$(ls -1 "$RUN_DIR"/*.log 2>/dev/null | grep -v -E '(writer|logger_stage|depthchart|colorresults|shader|intersect_results|proxy)\.log$' || true)
if [[ -n "$others" ]]; then
	for f in $others; do
		tail_if "$f"
	done
fi

# Show tails of primary output channel logs when present
tail_if "$RUN_DIR/oDEPTH.txt"
tail_if "$RUN_DIR/oPNG.txt"

echo "[pretty] Done."
