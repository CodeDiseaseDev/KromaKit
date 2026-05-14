#!/usr/bin/env bash
set -euo pipefail

build_platform() {
  local platform="$1"
  case "$platform" in
    ios)
      run make -C "$PROJECT_ROOT" ios BUILD="$SYNTHEM_MODE" -j"$SYNTHEM_JOBS"
      ;;
    macos)
      run make -C "$PROJECT_ROOT" macos BUILD="$SYNTHEM_MODE" -j"$SYNTHEM_JOBS"
      ;;
    linux)
      run make -C "$PROJECT_ROOT" linux -j"$SYNTHEM_JOBS"
      ;;
    windows)
      run make -C "$PROJECT_ROOT" windows -j"$SYNTHEM_JOBS"
      ;;
    *)
      die "unsupported build platform: $platform"
      ;;
  esac
}

cmd_build() {
  local target="${1:-all}"
  target="$(normalize_platform "$target")" || die "invalid build target: $target"

  log "target: $target"
  log "mode: $SYNTHEM_MODE"
  log "jobs: $SYNTHEM_JOBS"

  local build_targets
  build_targets="$(build_targets_for "$target")" || die "invalid build target: $target"

  local total=0
  local platform
  for platform in $build_targets; do
    total=$((total + 1))
  done

  local i=0
  for platform in $build_targets; do
    i=$((i + 1))
    step "$i" "$total" "build $platform"
    build_platform "$platform"
  done
}
