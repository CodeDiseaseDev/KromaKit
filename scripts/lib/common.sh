#!/usr/bin/env bash
set -euo pipefail

if [[ -t 1 ]]; then
  _SYNTHEM_CLR_INFO=$'\033[1;34m'
  _SYNTHEM_CLR_WARN=$'\033[1;33m'
  _SYNTHEM_CLR_ERR=$'\033[1;31m'
  _SYNTHEM_CLR_DIM=$'\033[2m'
  _SYNTHEM_CLR_RST=$'\033[0m'
else
  _SYNTHEM_CLR_INFO=""
  _SYNTHEM_CLR_WARN=""
  _SYNTHEM_CLR_ERR=""
  _SYNTHEM_CLR_DIM=""
  _SYNTHEM_CLR_RST=""
fi

log() {
  echo "${_SYNTHEM_CLR_INFO}[synthem]${_SYNTHEM_CLR_RST} $*"
}

warn() {
  echo "${_SYNTHEM_CLR_WARN}[synthem][warn]${_SYNTHEM_CLR_RST} $*" >&2
}

die() {
  echo "${_SYNTHEM_CLR_ERR}[synthem][error]${_SYNTHEM_CLR_RST} $*" >&2
  exit 1
}

run() {
  echo "${_SYNTHEM_CLR_DIM}[synthem][run]${_SYNTHEM_CLR_RST} $*"
  "$@"
}

step() {
  local idx="$1"
  local total="$2"
  shift 2
  log "step ${idx}/${total}: $*"
}

legacy_tool() {
  local script_name="$1"
  local script_path="$OLD_BUILD_TOOLS_DIR/$script_name"
  [[ -f "$script_path" ]] || die "missing legacy tool: $script_path"
  run bash "$script_path"
}

is_integer() {
  [[ "${1:-}" =~ ^[0-9]+$ ]]
}
