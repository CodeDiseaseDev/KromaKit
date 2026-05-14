#!/usr/bin/env bash
set -euo pipefail

cmd_logs() {
  local target="${1:-ios}"
  target="$(normalize_platform "$target")" || die "invalid logs target: $target"

  case "$target" in
    ios) legacy_tool view_live_logs.sh ;;
    *) die "logs currently supports only ios" ;;
  esac
}
