#!/usr/bin/env bash
set -euo pipefail

JOBS="${JOBS:-32}"

#clean
make clean-linux \
     clean-windows \
     clean-ios \
     clean-macos

#build
make linux windows ios macos -j"$JOBS"

./full_package.sh
