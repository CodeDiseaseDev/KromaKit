#!/usr/bin/env bash
set -euo pipefail

_release_one_platform() {
  local target="$1"

  local total=2
  local current_step=1

  if [[ "$SYNTHEM_DO_CLEAN" == "1" ]]; then
    total=3
    step "$current_step" "$total" "clean $target"
    cmd_clean "$target"
    current_step=$((current_step + 1))
  fi

  step "$current_step" "$total" "build $target"
  build_platform "$target"
  current_step=$((current_step + 1))

  step "$current_step" "$total" "package $target"

  case "$target" in
    ios)
      package_ios_native
      log "release ipa: $IOS_IPA_PATH"
      ;;

    macos)
      package_macos_native
      log "release app: $MACOS_APP_PATH"

      if [[ "$SYNTHEM_DO_DMG" == "1" ]]; then
        log "release image: $MACOS_DMG_HFS_PATH"
      fi
      ;;

    linux)
      package_linux_native
      log "release linux package: $BUILD_DIR_FULL_PACKAGE/$BUILD_LINUX_DIR"
      ;;

    windows)
      package_windows_native
      log "release windows package: $BUILD_DIR_FULL_PACKAGE/$BUILD_WINDOWS_DIR"
      ;;

    *)
      die "release target not implemented: $target"
      ;;
  esac
}

cmd_release() {
  local target="${1:-all}"
  target="$(normalize_platform "$target")" || die "invalid release target: $target"

  if [[ "${SYNTHEM_MODE_SET_BY_FLAG:-0}" != "1" ]]; then
    SYNTHEM_MODE="release"
  fi

  log "target: $target"
  log "mode: $SYNTHEM_MODE"
  log "jobs: $SYNTHEM_JOBS"

  case "$target" in
    ios|macos|linux|windows)
      _release_one_platform "$target"
      ;;

    all)
      local total=2
      local current_step=1

      if [[ "$SYNTHEM_DO_CLEAN" == "1" ]]; then
        total=3
        step "$current_step" "$total" "clean all"
        cmd_clean all
        current_step=$((current_step + 1))
      fi

      step "$current_step" "$total" "build all"
      cmd_build all
      current_step=$((current_step + 1))

      step "$current_step" "$total" "package all"
      package_all_native

      log "release package dir: $BUILD_DIR_FULL_PACKAGE"
      ;;

    *)
      die "release supports ios|macos|linux|windows|all"
      ;;
  esac
}
