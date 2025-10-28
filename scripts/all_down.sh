#!/usr/bin/env bash
# Robust teardown for pipeline/proxy processes, including relative ../bin/* and shell starters.
# Safe to run multiple times.
set -euo pipefail

# Resolve repo root (script is in scripts/)
ROOT_DIR=$(cd "$(dirname "$0")/.." && pwd)

NAMES=(
  ControlChannel
  AllSteps
  Writer
  DepthChart
  ColorResults
  Background
  Black
  Lit
  Shader
  IntersectResults
  IntersectWith
  PixelFactory
  Reflection
  Feeder
)

PORTS=(1300 1301 1303 1307 1308 1312 1313 1314)

log() { printf '%s\n' "$*"; }

list_pids_by_pattern() {
  local pattern="$1"
  pgrep -f "$pattern" 2>/dev/null || true
}

force_kill_pids() {
  local pids="$1"
  [ -z "$pids" ] && return 0
  echo "$pids" | xargs -n1 -I{} sh -c 'kill -9 {} 2>/dev/null || true' || true
}

term_then_kill() {
  local pids="$1"
  [ -z "$pids" ] && return 0
  echo "$pids" | xargs -n1 -I{} sh -c 'kill {} 2>/dev/null || true' || true
  sleep 0.2
  # re-check still alive
  local remain="$(echo "$pids" | xargs -n1 -I{} sh -c 'kill -0 {} 2>/dev/null && echo {}' || true)"
  if [ -n "$remain" ]; then
    force_kill_pids "$remain"
  fi
}

# 1) Kill by known binary names (matches ../bin/Name and Name arguments)
for n in "${NAMES[@]}"; do
  log "[all_down] stopping $n"
  pids=$(list_pids_by_pattern "(^|/|\\s)${n}(\\s|$)")
  term_then_kill "$pids"
done

# 2) Kill shell starters within this repo: '/bin/sh ./start.sh' with cwd under ROOT_DIR
if command -v lsof >/dev/null 2>&1; then
  start_pids=$(list_pids_by_pattern "/bin/sh ./start.sh")
  if [ -n "${start_pids}" ]; then
    for pid in ${start_pids}; do
      cwd=$(lsof -a -p "$pid" -d cwd -Fn 2>/dev/null | sed -n 's/^n//p' | tail -n1 || true)
      if [ -n "$cwd" ] && [[ "$cwd" == "$ROOT_DIR"* ]]; then
        log "[all_down] stopping start.sh pid $pid (cwd=$cwd)"
        term_then_kill "$pid"
      fi
    done
  fi
fi

# 3) Kill any process whose cmdline includes '../bin/' and whose cwd is under ROOT_DIR (relative launches)
if command -v lsof >/dev/null 2>&1; then
  rel_pids=$(list_pids_by_pattern "\.\./bin/")
  if [ -n "$rel_pids" ]; then
    for pid in ${rel_pids}; do
      cwd=$(lsof -a -p "$pid" -d cwd -Fn 2>/dev/null | sed -n 's/^n//p' | tail -n1 || true)
      if [ -n "$cwd" ] && [[ "$cwd" == "$ROOT_DIR"* ]]; then
        log "[all_down] stopping relative-bin pid $pid (cwd=$cwd)"
        term_then_kill "$pid"
      fi
    done
  fi
fi

# 4) Free known ports (best-effort); this catches both binders and connected peers
if command -v lsof >/dev/null 2>&1; then
  for p in "${PORTS[@]}"; do
    pids_on_port=$(lsof -ti tcp:"$p" 2>/dev/null || true)
    if [ -n "$pids_on_port" ]; then
      log "[all_down] killing pids on port $p: $(echo "$pids_on_port" | tr '\n' ' ')"
      term_then_kill "$pids_on_port"
    fi
  done
fi

# 5) Final sweep for any repo-local processes
final_pids=$(pgrep -af "${ROOT_DIR}|\.\./bin/|/bin/sh ./start.sh" 2>/dev/null || true)
if [ -n "$final_pids" ]; then
  log "[all_down] warning: some processes may still be alive (showing PID and cmd):"
  # Show PID and command
  pgrep -af "${ROOT_DIR}|\.\./bin/|/bin/sh ./start.sh" || true
else
  log "[all_down] all matched processes terminated"
fi

log "[all_down] complete"
