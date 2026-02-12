#!/usr/bin/env bash
# Compare a new run directory against runs/complete for known artifacts
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/../.. && pwd)"
BASE_COMPLETE="$ROOT_DIR/runs/complete"
RUN_DIR="${1:-}"
if [[ -z "$RUN_DIR" || ! -d "$RUN_DIR" ]]; then
	echo "Usage: $0 <run_dir>" >&2; exit 2
fi

artifacts=(oIntersectResult.txt oShader.txt oBKG.txt oBLACK.txt oLIT.txt oCOLOR.txt oDEPTH.txt oPNG.txt)
for a in "${artifacts[@]}"; do
	A="$BASE_COMPLETE/$a"
	B="$RUN_DIR/$a"
	if [[ -f "$A" && -f "$B" ]]; then
		echo "== $a =="
		echo " complete: $(wc -l < "$A") lines"
		echo " new     : $(wc -l < "$B") lines"
		if command -v diff >/dev/null; then
			diff -u "$A" "$B" | sed -n '1,100p'
		fi
	else
		echo "== $a == (missing in one side)"
		[[ -f "$A" ]] || echo " complete: MISSING"
		[[ -f "$B" ]] || echo " new     : MISSING"
	fi
	echo
done
