#!/usr/bin/env bash
set -euo pipefail

./increment_build.sh

bear -- make release

#JOBS="${JOBS:-$(nproc)}"
#
#
#
#if [[ "${REFRESH_COMPILE_DB:-0}" == "1" ]]; then
#  bear -- make -j"$JOBS" BUILD=debug
#else
#  make -j"$JOBS" BUILD=debug
#fi
