#!/usr/bin/env bash
set -euo pipefail

normalize_platform() {
  local p="${1:-all}"
  case "$p" in
    ios|macos|linux|windows|all) echo "$p" ;;
    *) return 1 ;;
  esac
}

build_targets_for() {
  local target="$1"
  case "$target" in
    all) echo "ios macos linux windows" ;;
    ios|macos|linux|windows) echo "$target" ;;
    *) return 1 ;;
  esac
}

package_targets_for() {
  local target="$1"
  case "$target" in
    all) echo "ios macos all" ;;
    ios|macos) echo "$target" ;;
    *) return 1 ;;
  esac
}

release_targets_for() {
  local target="$1"
  case "$target" in
    all) echo "all" ;;
    ios|macos) echo "$target" ;;
    *) return 1 ;;
  esac
}
