#!/bin/bash

# Check if the first argument is "--build"
if [[ "$1" == "--build" ]]; then
  # If it is, build the project and return
  shift
  ./build.sh 
fi

# Set the executable path
EXE_UNDER_TEST="./bin/AllSteps"

# Best-effort cleanup: kill stale AllSteps and free Cucumber wire port (3902)
{
  # Kill by executable path
  pkill -f "/RayTracer/test/bin/AllSteps" >/dev/null 2>&1 || true
  # Kill anything bound on port 3902 (macOS and Linux compatible)
  if command -v lsof >/dev/null 2>&1; then
    PIDS=$(lsof -ti tcp:3902 2>/dev/null || true)
    if [[ -n "$PIDS" ]]; then
      echo "found processes on port 3902... killing"
      kill -9 $PIDS >/dev/null 2>&1 || true
    fi
  fi
} || true

# Start the executable in the background and capture PID
"$EXE_UNDER_TEST" &
ALLSTEPS_PID=$!

# Ensure cleanup on exit
trap 'kill "$ALLSTEPS_PID" >/dev/null 2>&1 || true' EXIT

# Give the server a moment to bind
sleep 1

# Set the test to run based on the first argument
TEST_TO_RUN="*"
if [[ -n "$1" ]]; then
  TEST_TO_RUN="$1"
fi

# Run the Cucumber tests
cucumber --tags ~@skip features/$TEST_TO_RUN.feature

# Explicitly cleanup the background server
kill "$ALLSTEPS_PID" >/dev/null 2>&1 || true
wait "$ALLSTEPS_PID" >/dev/null 2>&1 || true
