#!/usr/bin/env bash
# Regenerate data/pixels.txt using the current camera math from PixelFactory
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"

echo "[regen] Building and running PixelFactory to produce output.txt..."
# Build using repo script (must run from repo root)
if [[ -x "$ROOT_DIR/build.sh" ]]; then
  ( cd "$ROOT_DIR" && ./build.sh >/dev/null )
fi

# Run PixelFactory to emit output.txt
( cd "$ROOT_DIR/PixelFactory" && ./start.sh >/dev/null )

SRC="$ROOT_DIR/PixelFactory/output.txt"
DST="$ROOT_DIR/data/pixels.txt"

if [[ ! -s "$SRC" ]]; then
  echo "[regen] ERROR: Missing PixelFactory/output.txt; aborting." >&2
  exit 1
fi

echo "[regen] Copying $SRC -> $DST"
cp "$SRC" "$DST"

echo "[regen] Verifying with scripts/validation/compare_pixels.py (expect MATCH)"
if /usr/bin/python3 "$ROOT_DIR/scripts/validation/compare_pixels.py"; then
  echo "[regen] SUCCESS: data/pixels.txt matches current PixelFactory rays."
else
  echo "[regen] WARNING: compare script reported mismatches. Inspect differences above." >&2
  exit 1
fi

echo "[regen] Done."
