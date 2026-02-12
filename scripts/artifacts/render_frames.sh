#!/usr/bin/env bash
# Render PNGs for each step artifact using Writer (no PIL). The output will be colored via Writer's BRUTE_FORCE path.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
RUN_DIR="${1:-}"
OUT_DIR="${2:-}"
if [[ -z "$RUN_DIR" || ! -d "$RUN_DIR" ]]; then echo "Usage: $0 <run_dir> <out_dir>"; exit 2; fi
mkdir -p "$OUT_DIR"
STAGE2PNG="$ROOT_DIR/scripts/tools/stage_to_png.sh"
render() { local f=$1; local out=$2; if [[ -s "$f" ]]; then "$STAGE2PNG" "$out" "$f"; else echo "skip $f (missing or empty)"; fi }

render "$RUN_DIR/oIntersectResult.txt" "$OUT_DIR/step2_intersect.png"
render "$RUN_DIR/oShader.txt" "$OUT_DIR/step3_shader.png"
render "$RUN_DIR/oBKG.txt" "$OUT_DIR/step2_bkg.png"
render "$RUN_DIR/oBLACK.txt" "$OUT_DIR/step2_black.png"
render "$RUN_DIR/oLIT.txt" "$OUT_DIR/step2_lit.png"
render "$RUN_DIR/oCOLOR.txt" "$OUT_DIR/step3_color.png"
render "$RUN_DIR/oDEPTH.txt" "$OUT_DIR/step4_depth.png"
render "$RUN_DIR/oPNG.txt" "$OUT_DIR/step5_pngchannel.png"
# final PNG already exists as test.png inside run dir (from Writer)
