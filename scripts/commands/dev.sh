#!/usr/bin/env bash
set -euo pipefail

cmd_dev() {
  local target="${1:-ios}"
  target="$(normalize_platform "$target")" || die "invalid dev target: $target"

  if [[ "${SYNTHEM_MODE_SET_BY_FLAG:-0}" != "1" ]]; then
    SYNTHEM_MODE="debug"
  fi

  log "target: $target"
  log "mode: $SYNTHEM_MODE"
  log "jobs: $SYNTHEM_JOBS"

  if [[ "$target" != "ios" ]]; then
    cmd_build "$target"
    return 0
  fi

  local total=3
  if [[ "$SYNTHEM_DO_INSTALL" == "1" ]]; then
    total=4
  fi

  local idx=1
  if [[ "$SYNTHEM_DO_CLEAN" == "1" ]]; then
    step "$idx" "$total" "clean ios"
    cmd_clean ios
    idx=$((idx + 1))
  fi

  step "$idx" "$total" "build ios"
  build_platform ios
  idx=$((idx + 1))

  step "$idx" "$total" "package ios"
  package_ios_native
  idx=$((idx + 1))

  if [[ "$SYNTHEM_DO_INSTALL" == "1" ]]; then
    step "$idx" "$total" "deploy to device"
    cmd_deploy ios
  else
    log "deploy skipped (--no-install)"
  fi
}
