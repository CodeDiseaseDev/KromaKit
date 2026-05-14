#!/usr/bin/env bash
set -euo pipefail

_zip_dir_contents() {
  local source_dir="$1"
  local output_zip="$2"

  [[ -d "$source_dir" ]] || die "missing directory to zip: $source_dir"

  run mkdir -p "$(dirname "$output_zip")"
  run rm -f "$output_zip"

  (
    cd "$source_dir"
    run zip -r "$output_zip" . >/dev/null
  )

  [[ -f "$output_zip" ]] || die "failed to create zip: $output_zip"
  log "zip ready: $output_zip"
}

zip_ios_native() {
  [[ -d "$IOS_APP_DIR" ]] || die "missing iOS app dir: $IOS_APP_DIR (run package ios first)"

  local output_zip="${1:-$BUILD_DIR_FULL_PACKAGE/${APP_NAME}-ios.zip}"

  run mkdir -p "$BUILD_DIR_FULL_PACKAGE"
  _zip_dir_contents "$IOS_APP_DIR" "$output_zip"
}

zip_macos_native() {
  [[ -d "$MACOS_APP_PATH" ]] || die "missing macOS app: $MACOS_APP_PATH (run package macos first)"

  local output_zip="${1:-$BUILD_DIR_FULL_PACKAGE/${APP_NAME}-macos.zip}"

  run mkdir -p "$BUILD_DIR_FULL_PACKAGE"
  run rm -f "$output_zip"

  (
    cd "$(dirname "$MACOS_APP_PATH")"
    run zip -r "$output_zip" "$(basename "$MACOS_APP_PATH")" >/dev/null
  )

  [[ -f "$output_zip" ]] || die "failed to create zip: $output_zip"
  log "zip ready: $output_zip"
}

zip_linux_native() {
  local full_package_linux_dir="$BUILD_DIR_FULL_PACKAGE/$BUILD_LINUX_DIR"
  [[ -d "$full_package_linux_dir" ]] ||
    die "missing Linux package dir: $full_package_linux_dir (run package all or package linux first)"

  local output_zip="${1:-$BUILD_DIR_FULL_PACKAGE/${APP_NAME}-linux.zip}"

  _zip_dir_contents "$full_package_linux_dir" "$output_zip"
}

zip_windows_native() {
  local full_package_windows_dir="$BUILD_DIR_FULL_PACKAGE/$BUILD_WINDOWS_DIR"
  [[ -d "$full_package_windows_dir" ]] ||
    die "missing Windows package dir: $full_package_windows_dir (run package windows first)"

  local output_zip="${1:-$BUILD_DIR_FULL_PACKAGE/${APP_NAME}-windows.zip}"

  _zip_dir_contents "$full_package_windows_dir" "$output_zip"
}

zip_all_native() {
  local output_zip="${1:-$BUILD_DIR_FULL_PACKAGE/${APP_NAME}-all.zip}"

  [[ -d "$BUILD_DIR_FULL_PACKAGE" ]] ||
    die "missing full package dir: $BUILD_DIR_FULL_PACKAGE (run package all first)"

  run rm -f "$output_zip"

  (
    cd "$(dirname "$BUILD_DIR_FULL_PACKAGE")"
    run zip -r "$output_zip" "$(basename "$BUILD_DIR_FULL_PACKAGE")" >/dev/null
  )

  [[ -f "$output_zip" ]] || die "failed to create zip: $output_zip"
  log "zip ready: $output_zip"
}

cmd_zip() {
  local target="${1:-all}"
  local output_zip="${2:-}"

  target="$(normalize_platform "$target")" || die "invalid zip target: $target"

  [[ $# -le 2 ]] || die "zip accepts at most: target and output path"

  log "target: $target"

  case "$target" in
    ios)
      step 1 1 "zip ios"
      zip_ios_native "$output_zip"
      ;;
    macos)
      step 1 1 "zip macos"
      zip_macos_native "$output_zip"
      ;;
    linux)
      step 1 1 "zip linux"
      zip_linux_native "$output_zip"
      ;;
    windows)
      step 1 1 "zip windows"
      zip_windows_native "$output_zip"
      ;;
    all)
      step 1 1 "zip all"
      zip_all_native "$output_zip"
      ;;
    *)
      die "invalid zip target: $target"
      ;;
  esac
}
