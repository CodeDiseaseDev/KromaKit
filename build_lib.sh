#!/usr/bin/env bash
set -euo pipefail

rm -rf /home/code/Coding/KromaKit/cmake-build-debug

cmake -S /home/code/Coding/KromaKit \
  -B /home/code/Coding/KromaKit/cmake-build-debug \
  -G Ninja \
  -DKROMAKIT_BUILD_EXAMPLES=ON

cmake --build /home/code/Coding/KromaKit/cmake-build-debug -j32

