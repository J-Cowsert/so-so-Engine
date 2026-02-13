#!/bin/bash
# Quick build script for Linux

set -e

CONFIG=${1:-Debug}

echo "Building so-so Engine ($CONFIG)..."

cmake -B build -S . -DCMAKE_BUILD_TYPE=$CONFIG
cmake --build build -j$(nproc)

echo ""
echo "Build complete!"
echo "Run with: ./build/bin/$CONFIG-Linux-x64/Sandbox/Sandbox"
