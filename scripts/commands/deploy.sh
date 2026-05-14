#!/usr/bin/env bash
set -euo pipefail

cmd_deploy() {
  local target="${1:-ios}"
  target="$(normalize_platform "$target")" || die "invalid deploy target: $target"

  case "$target" in
    ios)
      if [[ "$SYNTHEM_DO_INSTALL" != "1" ]]; then
        log "deploy skipped (--no-install)"
        return 0
      fi
      legacy_tool compile_send.sh
      ;;
    *)
      die "deploy currently supports only ios"
      ;;
  esac
}
