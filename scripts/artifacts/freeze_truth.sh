#!/usr/bin/env bash
# Freeze the latest per-object run into a canonical runs/truth folder and compare to runs/complete.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
source "$ROOT_DIR/scripts/tools/pipeline.cfg.sh"

LATEST_RUN="${1:-}"
if [[ -z "${LATEST_RUN}" ]]; then
  LATEST_RUN=$(ls -dt "$ROOT_DIR"/runs/objects-* 2>/dev/null | head -n1 || true)
fi
if [[ -z "${LATEST_RUN}" || ! -d "${LATEST_RUN}" ]]; then
  echo "No per-object run found. Provide a run dir: $0 runs/objects-YYYYmmdd-HHMMSS" >&2
  exit 2
fi

TRUTH_DIR="$ROOT_DIR/runs/truth"
OBJ_DIR="$TRUTH_DIR/objects"
mkdir -p "$OBJ_DIR"

echo "[freeze] Using run: $LATEST_RUN"
echo "[freeze] Writing truth artifacts to: $TRUTH_DIR"

# Python helper to de-dup two-line Pixel+Intersection records by (x,y)
DEDUP_PY="${TMPDIR:-/tmp}/dedup_intersect.$$.$RANDOM.py"
cat > "$DEDUP_PY" <<'PY'
import sys
seen=set()
i=sys.stdin
o=sys.stdout
while True:
    line1=i.readline()
    if not line1:
        break
    line2=i.readline()
    if not line2:
        break
    try:
        x,y=line1.split(',',2)[:2]
        key=(int(float(x)), int(float(y)))
    except Exception:
        key=line1
    if key in seen:
        continue
    seen.add(key)
    o.write(line1)
    o.write(line2)
PY

# 1) Copy per-object IntersectWith artifacts (dedup if necessary)
for OBJ in "${OBJECTS[@]}"; do
  src="$LATEST_RUN/$OBJ/oIntersectResult.$OBJ.txt"
  dst="$OBJ_DIR/oIntersectResult.$OBJ.txt"
  if [[ -f "$src" ]]; then
    lines=$(wc -l < "$src" | tr -d ' ')
    if [[ "$lines" -gt 40000 ]]; then
      echo "[freeze] $OBJ appears duplicated ($lines lines), de-duplicating by (x,y)..."
      python3 "$DEDUP_PY" < "$src" > "$dst"
    else
      cp -f "$src" "$dst"
    fi
  else
    echo "[freeze] WARN: missing per-object artifact: $src"
  fi
done

# 2) Copy stage outputs if present
for a in oCOLOR.txt oDEPTH.txt oPNG.txt test.png; do
  [[ -f "$LATEST_RUN/$a" ]] && cp -f "$LATEST_RUN/$a" "$TRUTH_DIR/$a" || true
done

# 3) Write a manifest (line counts)
{
  echo "run_dir=$LATEST_RUN"
  echo "timestamp=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  for a in oCOLOR.txt oDEPTH.txt oPNG.txt; do
    if [[ -f "$TRUTH_DIR/$a" ]]; then
      echo "$a: $(wc -l < "$TRUTH_DIR/$a")"
    fi
  done
  for OBJ in "${OBJECTS[@]}"; do
    f="$OBJ_DIR/oIntersectResult.$OBJ.txt"
    [[ -f "$f" ]] && echo "objects/$OBJ: $(wc -l < "$f")" || true
  done
} > "$TRUTH_DIR/manifest.txt"

echo "[freeze] Wrote manifest: $TRUTH_DIR/manifest.txt"

# 4) Compare truth vs runs/complete for common artifacts
if [[ -d "$ROOT_DIR/runs/complete" ]]; then
  echo "[freeze] Comparing truth vs runs/complete..."
  "$ROOT_DIR/scripts/validation/compare_artifacts.sh" "$TRUTH_DIR" || true
else
  echo "[freeze] Skipping compare; runs/complete missing"
fi

echo "[freeze] Done."
