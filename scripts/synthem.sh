#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$SCRIPT_DIR/lib/paths.sh"
# shellcheck source=scripts/lib/config.sh
source "$SCRIPT_DIR/lib/config.sh"
# shellcheck source=scripts/lib/common.sh
source "$SCRIPT_DIR/lib/common.sh"
# shellcheck source=scripts/lib/platforms.sh
source "$SCRIPT_DIR/lib/platforms.sh"
# shellcheck source=scripts/commands/build.sh
source "$SCRIPT_DIR/commands/build.sh"
# shellcheck source=scripts/commands/icons.sh
source "$SCRIPT_DIR/commands/icons.sh"
# shellcheck source=scripts/commands/plist.sh
source "$SCRIPT_DIR/commands/plist.sh"
# shellcheck source=scripts/commands/package.sh
source "$SCRIPT_DIR/commands/package.sh"
# shellcheck source=scripts/commands/clean.sh
source "$SCRIPT_DIR/commands/clean.sh"
# shellcheck source=scripts/commands/deploy.sh
source "$SCRIPT_DIR/commands/deploy.sh"
# shellcheck source=scripts/commands/logs.sh
source "$SCRIPT_DIR/commands/logs.sh"
# shellcheck source=scripts/commands/dev.sh
source "$SCRIPT_DIR/commands/dev.sh"
# shellcheck source=scripts/commands/release.sh
source "$SCRIPT_DIR/commands/release.sh"
# shellcheck source=scripts/commands/zip.sh
source "$SCRIPT_DIR/commands/zip.sh"

cd "$PROJECT_ROOT"

COMMAND="${1:-help}"
SCRIPT_START_TIME="$(date +%s)"

print_elapsed_time() {
  local exit_code="$?"
  local end_time elapsed minutes seconds

  trap - EXIT

  end_time="$(date +%s)"
  elapsed=$((end_time - SCRIPT_START_TIME))
  minutes=$((elapsed / 60))
  seconds=$((elapsed % 60))

  case "${COMMAND:-help}" in
    help|"")
      ;;
    *)
      if [[ "$exit_code" -eq 0 ]]; then
        log "done in ${minutes}m ${seconds}s"
      else
        warn "failed after ${minutes}m ${seconds}s"
      fi
      ;;
  esac

  return "$exit_code"
}

trap print_elapsed_time EXIT

SYNTHEM_MODE_SET_BY_FLAG=0

show_help() {
  cat <<'EOF'
Usage:
  ./synthem.sh <command> [target] [flags]

Commands:
  help
  build    ios|macos|linux|windows|all
  package  ios|macos|all
  dev      ios|macos|linux|windows|all
  release  ios|macos|all
  clean    ios|macos|linux|windows|all
  icons    ios|macos|all
  plist    [output-path]
  logs     ios
  deploy   ios
  zip      ios|macos|linux|windows|all [/path/to/zip.zip]

Flags:
  --clean
  --debug
  --release
  --jobs N
  --no-install
  --dmg

Examples:
  ./synthem.sh help
  ./synthem.sh dev ios
  ./synthem.sh release ios
  ./synthem.sh release macos --dmg
  ./synthem.sh release all --jobs 32
  ./synthem.sh build ios --debug
  ./synthem.sh package ios
  ./synthem.sh logs ios
  ./synthem.sh clean
  ./synthem.sh zip windows
  ./synthem.sh zip all
  ./synthem.sh zip ios /tmp/Synthem-ios.zip
EOF
}




if [[ $# -gt 0 ]]; then
  shift
fi

TARGET=""
EXTRA_ARGS=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    --clean)
      SYNTHEM_DO_CLEAN=1
      ;;
    --debug)
      SYNTHEM_MODE="debug"
      SYNTHEM_MODE_SET_BY_FLAG=1
      ;;
    --release)
      SYNTHEM_MODE="release"
      SYNTHEM_MODE_SET_BY_FLAG=1
      ;;
    --jobs)
      shift
      [[ $# -gt 0 ]] || die "--jobs requires a number"
      is_integer "$1" || die "--jobs requires an integer"
      SYNTHEM_JOBS="$1"
      ;;
    --jobs=*)
      SYNTHEM_JOBS="${1#*=}"
      is_integer "$SYNTHEM_JOBS" || die "--jobs requires an integer"
      ;;
    -j)
      shift
      [[ $# -gt 0 ]] || die "-j requires a number"
      is_integer "$1" || die "-j requires an integer"
      SYNTHEM_JOBS="$1"
      ;;
    --no-install)
      SYNTHEM_DO_INSTALL=0
      ;;
    --dmg)
      SYNTHEM_DO_DMG=1
      ;;
    --theme=*)
      OVERRIDE_THEME="${1#*=}"
      ;;
    -h|--help)
      COMMAND="help"
      ;;
    *)
      if [[ -z "$TARGET" ]]; then
        TARGET="$1"
      else
        EXTRA_ARGS+=("$1")
      fi
      ;;
  esac
  shift
done

export PROJECT_ROOT OLD_BUILD_TOOLS_DIR APP_NAME BUNDLE_ID
export BUILD_DIR_IOS BUILD_DIR_IOS_PACKAGE BUILD_DIR_MACOS BUILD_DIR_MACOS_PACKAGE
export BUILD_DIR_LINUX BUILD_DIR_WINDOWS BUILD_DIR_FULL_PACKAGE BUILD_DIR_ICONS
export IOS_IPA_PATH IOS_RAWEXEC_PATH IOS_APP_DIR
export MACOS_APP_PATH MACOS_DMG_HFS_PATH MACOS_DMG_PATH MACOS_ICNS_PATH
export RESOURCE_DIR ICON_SVG_PATH FONT_DIR
export IOS_TOOLCHAIN_BIN MACOS_TOOLCHAIN_BIN IOS_LDID_BIN MACOS_LDID_BIN
export SYNTHEM_MODE SYNTHEM_JOBS SYNTHEM_DO_INSTALL SYNTHEM_DO_DMG SYNTHEM_DO_CLEAN SYNTHEM_MODE_SET_BY_FLAG
export OVERRIDE_THEME

case "$COMMAND" in
  help|"")
    show_help
    ;;
  build)
    cmd_build "${TARGET:-all}"
    ;;
  package)
    cmd_package "${TARGET:-all}"
    ;;
  dev)
    cmd_dev "${TARGET:-ios}"
    ;;
  release)
    cmd_release "${TARGET:-all}"
    ;;
  clean)
    cmd_clean "${TARGET:-all}"
    ;;
  icons)
    cmd_icons "${TARGET:-all}" "${EXTRA_ARGS[@]}"
    ;;
  plist)
    if [[ ${#EXTRA_ARGS[@]} -gt 0 ]]; then
      die "plist accepts at most one output path"
    fi
    cmd_plist "${TARGET:-$IOS_APP_DIR/Info.plist}"
    ;;
  logs)
    cmd_logs "${TARGET:-ios}"
    ;;
  deploy)
    cmd_deploy "${TARGET:-ios}"
    ;;
  zip)
    cmd_zip "${TARGET:-all}" "${EXTRA_ARGS[@]}"
    ;;
  *)
    die "unknown command: $COMMAND (run './synthem.sh help')"
    ;;
esac
