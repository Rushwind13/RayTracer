#!/usr/bin/env bash
# Convenience: show a few example lines from an intersections artifact
set -euo pipefail
FILE="${1:-}"
if [[ -z "$FILE" || ! -f "$FILE" ]]; then echo "Usage: $0 <oIntersectResult*.txt>" >&2; exit 2; fi
paste - - < "$FILE" | head -n 5
