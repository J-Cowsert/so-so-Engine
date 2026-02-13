#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CONFIG=${1:-Debug}
SANDBOX_DIR="$PROJECT_ROOT/build/bin/$CONFIG-Linux-x64/Sandbox"

if [ ! -f "$SANDBOX_DIR/Sandbox" ]; then
    echo "Error: Sandbox executable not found at $SANDBOX_DIR/Sandbox"
    echo "Run scripts/build.sh first to build the project"
    exit 1
fi

echo "Launching Sandbox ($CONFIG)..."
cd "$SANDBOX_DIR"
LD_LIBRARY_PATH="." ./Sandbox
