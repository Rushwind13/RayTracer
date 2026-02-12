#!/usr/bin/env bash
# Re-render PNGs from an existing objects run using Writer only.
# Does NOT recalculate any .txt artifacts.
# Usage: rerender_from_artifacts.sh [runs/objects-YYYYmmdd-HHMMSS]
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
RUN_DIR="${1:-}"
if [[ -z "$RUN_DIR" ]]; then
  RUN_DIR=$(ls -dt "$ROOT_DIR"/runs/objects-* 2>/dev/null | head -n1 || true)
fi
if [[ -z "$RUN_DIR" || ! -d "$RUN_DIR" ]]; then
  echo "Usage: $0 <runs/objects-YYYYmmdd-HHMMSS>" >&2
  echo "No objects run found under $ROOT_DIR/runs" >&2
  exit 2
fi

echo "[rerender] Run directory: $RUN_DIR"

STAGE2PNG="$ROOT_DIR/scripts/tools/stage_to_png.sh"

# Re-render per-object PNGs
shopt -s nullglob
objs=("$RUN_DIR"/*)
for dir in "${objs[@]}"; do
  [[ -d "$dir" ]] || continue
  base=$(basename "$dir")
  case "$base" in
    box1|sphere2|sphere3|plane_floor|plane_l_wall|plane_r_wall)
      art="$dir/oIntersectResult.$base.txt"
      out="$dir/$base.png"
      if [[ -s "$art" ]]; then
        echo "[rerender] $base -> $out"
        "$STAGE2PNG" "$out" "$art" >/dev/null
      else
        echo "[rerender] skip $base (missing $art)"
      fi
      ;;
    *) ;;
  esac
done

# Re-render aggregate stage frames if present
if [[ -s "$RUN_DIR/oCOLOR.txt" ]]; then
  echo "[rerender] frame_color.png from oCOLOR.txt"
  "$STAGE2PNG" "$RUN_DIR/frame_color.png" "$RUN_DIR/oCOLOR.txt" >/dev/null
fi
if [[ -s "$RUN_DIR/oDEPTH.txt" ]]; then
  echo "[rerender] frame_depth.png from oDEPTH.txt"
  "$STAGE2PNG" "$RUN_DIR/frame_depth.png" "$RUN_DIR/oDEPTH.txt" >/dev/null
fi
if [[ -s "$RUN_DIR/oPNG.txt" ]]; then
  echo "[rerender] frame_png.png and test.png from oPNG.txt"
  "$STAGE2PNG" "$RUN_DIR/frame_png.png" "$RUN_DIR/oPNG.txt" >/dev/null
  "$STAGE2PNG" "$RUN_DIR/test.png" "$RUN_DIR/oPNG.txt" >/dev/null
fi

echo "[rerender] Done."
