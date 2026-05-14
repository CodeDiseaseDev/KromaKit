#!/usr/bin/env bash
set -euo pipefail

clean_platform() {
  local platform="$1"
  case "$platform" in
    ios) run make -C "$PROJECT_ROOT" clean-ios ;;
    macos) run make -C "$PROJECT_ROOT" clean-macos ;;
    linux) run make -C "$PROJECT_ROOT" clean-linux ;;
    windows) run make -C "$PROJECT_ROOT" clean-windows ;;
    all)
      run make -C "$PROJECT_ROOT" clean-linux clean-windows clean-ios clean-macos
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
