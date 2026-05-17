#!/usr/bin/env bash
set -euo pipefail

clean_platform() {
  local platform="$1"
  case "$platform" in
    ios) rm -rf build_ios ;;
    macos) rm -rf build_macos ;;
    linux) rm -rf build_linux ;;
    windows) rm -rf build_windows ;;
    all)
      rm -rf build_ios
      rm -rf build_macos
      rm -rf build_linux
      rm -rf build_windows
      ;;
    *)
      die "unsupported clean platform: $platform"
      ;;
  esac
}

cmd_clean() {
  local target="${1:-all}"
  target="$(normalize_platform "$target")" || die "invalid clean target: $target"
  log "target: $target"
  clean_platform "$target"
}
