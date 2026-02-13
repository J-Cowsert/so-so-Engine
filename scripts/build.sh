#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

CONFIG=${1:-Debug}

echo "Building so-so Engine ($CONFIG)..."

cmake -B "$PROJECT_ROOT/build" -S "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=$CONFIG
cmake --build "$PROJECT_ROOT/build" -j$(nproc)

echo ""
echo "Build complete!"
