#!/usr/bin/env bash
# Freeze the regenerated pixels (Stage 1 truth) into runs/truth
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"

SRC="$ROOT_DIR/data/pixels.txt"
TRUTH_DIR="$ROOT_DIR/runs/truth"
mkdir -p "$TRUTH_DIR"

if [[ ! -s "$SRC" ]]; then
  echo "[freeze-pixels] ERROR: Missing $SRC. Run scripts/regenerate_ref_pixels.sh first." >&2
  exit 1
fi

cp -f "$SRC" "$TRUTH_DIR/pixels.txt"

# Append or create a manifest with timestamp
{
  echo "stage1_pixels: $(wc -l < "$TRUTH_DIR/pixels.txt")"
  echo "timestamp: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
} >> "$TRUTH_DIR/manifest.txt"

echo "[freeze-pixels] Wrote $TRUTH_DIR/pixels.txt and updated manifest.txt"
