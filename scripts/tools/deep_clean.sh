#!/usr/bin/env bash
# Aggressively remove all object files (including Finder-style duplicates like "foo 2.o")
# and intermediate artifacts across the RayTracer tree.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
cd "$ROOT_DIR"

echo "[deep_clean] Removing *.o and duplicate * <n>.o files..."
find "$ROOT_DIR" -type f -name '*.o' -print -delete
find "$ROOT_DIR" -type f -name '* [0-9]*.o' -print -delete

echo "[deep_clean] Done. You can now rebuild modules (e.g., bash build.sh or cd ColorResults && make)."
