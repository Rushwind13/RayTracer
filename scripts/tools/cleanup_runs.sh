#!/usr/bin/env bash
# Prune runs older than newest truth artifacts and remove runtime logs from truth
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
RUNS_DIR="$ROOT_DIR/runs"
TRUTH_DIR="$RUNS_DIR/truth"
DRY=${DRY:-0}

# Find newest mtime in truth artifacts (txt/png only)
latest_truth_epoch=0
if [[ -d "$TRUTH_DIR" ]]; then
  while IFS= read -r -d '' f; do
    m=$(stat -f %m "$f" 2>/dev/null || stat -c %Y "$f" 2>/dev/null || echo 0)
    (( m > latest_truth_epoch )) && latest_truth_epoch=$m || true
  done < <(find "$TRUTH_DIR" -type f \( -name '*.txt' -o -name '*.png' \) -print0 2>/dev/null)
fi

echo "[cleanup] newest truth mtime: $latest_truth_epoch"

# 1) Remove runs older than truth
if [[ -d "$RUNS_DIR" ]]; then
  for d in "$RUNS_DIR"/*; do
    [[ -d "$d" ]] || continue
    [[ "$d" == "$TRUTH_DIR" ]] && continue
    m=$(stat -f %m "$d" 2>/dev/null || stat -c %Y "$d" 2>/dev/null || echo 0)
    if (( latest_truth_epoch > 0 && m > 0 && m < latest_truth_epoch )); then
      echo "[cleanup] prune old run: $d"
      if [[ "$DRY" == "0" ]]; then rm -rf "$d"; fi
    fi
  done
fi

# 2) Remove runtime logs from truth, keep artifacts
if [[ -d "$TRUTH_DIR" ]]; then
  while IFS= read -r -d '' f; do
    echo "[cleanup] remove truth log: $f"
    if [[ "$DRY" == "0" ]]; then rm -f "$f"; fi
  done < <(find "$TRUTH_DIR" -type f \( -name '*.log' -o -name '*.pid' \) -print0)
fi

echo "[cleanup] done"
