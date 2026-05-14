#!/usr/bin/env bash
set -euo pipefail

# get the electron rcedit windows binary from:
# https://github.com/electron/rcedit/releases
WINDOWS_RCEDIT_BIN="${WINDOWS_RCEDIT_BIN:-$PROJECT_ROOT/scripts/tools/rcedit-x64.exe}"

sync_ios_to_full_package() {
  local dst="$BUILD_DIR_FULL_PACKAGE/build_ios"

  run rm -rf "$dst"
  run mkdir -p "$dst"

  run cp -r "$IOS_APP_DIR" "$dst/${APP_NAME}.app"
  run cp "$IOS_IPA_PATH" "$dst/"
}

sync_macos_to_full_package() {
  local dst="$BUILD_DIR_FULL_PACKAGE/$BUILD_MACOS_DIR"

  run rm -rf "$dst/${APP_NAME}.app"
  run mkdir -p "$dst"

  run cp -r "$MACOS_APP_PATH" "$dst/${APP_NAME}.app"
}

embed_windows_icon() {
  cmd_icons windows

  local exe_path="$1"
  local icon_path="$BUILD_DIR_ICONS/windows/${APP_NAME}.ico"

  [[ -f "$exe_path" ]] || die "missing Windows exe: $exe_path"
  [[ -f "$icon_path" ]] || {
    log "warning: Windows icon missing: $icon_path"
    return 0
  }

  [[ -f "$WINDOWS_RCEDIT_BIN" ]] ||
    die "missing rcedit: $WINDOWS_RCEDIT_BIN"

  run wine "$WINDOWS_RCEDIT_BIN" "$exe_path" --set-icon "$icon_path"

#  if command -v "$WINDOWS_RCEDIT_BIN" >/dev/null 2>&1; then
#    run wine "$WINDOWS_RCEDIT_BIN" "$exe_path" --set-icon "$icon_path"
##    run rcedit "$exe_path" --set-icon "$icon_path"
#  elif command -v wine >/dev/null 2>&1 && command -v rcedit.exe >/dev/null 2>&1; then
#    run wine "$(command -v rcedit.exe)" "$exe_path" --set-icon "$icon_path"
#  else
#    log "warning: rcedit not found; skipping Windows exe icon embedding"
#  fi
}

package_ios_native() {
  cmd_icons ios

  [[ -f "$IOS_RAWEXEC_PATH" ]] || die "missing iOS binary: $IOS_RAWEXEC_PATH (run build ios first)"
  [[ -x "$IOS_LDID_BIN" ]] || die "missing ldid: $IOS_LDID_BIN"

  local iconset_dir="$BUILD_DIR_ICONS/ios/AppIcon.appiconset"
  [[ -d "$iconset_dir" ]] || cmd_icons ios
  [[ -d "$iconset_dir" ]] || die "missing iOS icon set: $iconset_dir"

  run rm -rf "$BUILD_DIR_IOS_PACKAGE/Payload" "$IOS_IPA_PATH"
  run mkdir -p "$IOS_APP_DIR"
  run cp "$IOS_RAWEXEC_PATH" "$IOS_APP_DIR/$APP_NAME"
  run chmod +x "$IOS_APP_DIR/$APP_NAME"

  run rsync -a --exclude='icon.png' "$RESOURCE_DIR/" "$IOS_APP_DIR/"
  run cp "$iconset_dir"/*.png "$IOS_APP_DIR/"
  generate_info_plist "$IOS_APP_DIR/Info.plist"

  run "$IOS_LDID_BIN" -S "$IOS_APP_DIR/$APP_NAME"
  run "$IOS_LDID_BIN" -S "$IOS_APP_DIR"

  (
    cd "$BUILD_DIR_IOS_PACKAGE"
    run zip -r "${APP_NAME}.ipa" Payload >/dev/null
  )

  log "iOS package: $IOS_IPA_PATH"
}

package_macos_native() {
  cmd_icons macos

  legacy_tool package_macos_app.sh

  local macos_icns_path="$BUILD_DIR_ICONS/macos/${APP_NAME}.icns"
  local macos_resources_dir="$MACOS_APP_PATH/Contents/Resources"
  local macos_icon_dest="$macos_resources_dir/${APP_NAME}.icns"

  run mkdir -p "$macos_resources_dir"

  if [[ -f "$macos_icns_path" ]]; then
    run cp "$macos_icns_path" "$macos_icon_dest"
    log "copied macOS icon: $macos_icon_dest"
  else
    log "warning: macOS icon missing: $macos_icns_path"
  fi

  if [[ "$SYNTHEM_DO_DMG" == "1" ]]; then
    legacy_tool package_macos_dmg.sh
  fi

  log "macOS package: $MACOS_APP_PATH"

  if [[ "$SYNTHEM_DO_DMG" == "1" ]]; then
    log "macOS image: $MACOS_DMG_HFS_PATH"
  fi
}

package_windows_native() {
  [[ -d "$BUILD_DIR_WINDOWS" ]] ||
    die "missing Windows build dir: $BUILD_DIR_WINDOWS (run build windows first)"

  local windows_package_dir="$BUILD_DIR_FULL_PACKAGE/$BUILD_WINDOWS_DIR"

  run rm -rf "$windows_package_dir"
  run mkdir -p "$windows_package_dir/resources"

  run rsync -a --exclude='icon.png' "$RESOURCE_DIR/" "$windows_package_dir/resources/"
  run rsync -a --exclude='obj/' "$BUILD_DIR_WINDOWS/" "$windows_package_dir/"

  embed_windows_icon "$windows_package_dir/${APP_NAME}.exe"

  log "Windows package: $windows_package_dir"
}

package_all_native() {
  local full_package_linux_dir="$BUILD_DIR_FULL_PACKAGE/$BUILD_LINUX_DIR"
  local full_package_macos_dir="$BUILD_DIR_FULL_PACKAGE/$BUILD_MACOS_DIR"

  package_ios_native
  package_macos_native

  run rm -rf "$BUILD_DIR_FULL_PACKAGE"
  run mkdir -p \
    "$full_package_linux_dir/resources" \
    "$full_package_macos_dir" \
    "$BUILD_DIR_FULL_PACKAGE/build_ios"

  run rsync -a --exclude='icon.png' "$RESOURCE_DIR/" "$full_package_linux_dir/resources/"
  run rsync -a --exclude='obj/' "$BUILD_DIR_LINUX/" "$full_package_linux_dir/"

  package_windows_native

  run cp -r "$MACOS_APP_PATH" "$full_package_macos_dir/${APP_NAME}.app"

  local src_bin="$MACOS_APP_PATH/Contents/MacOS/$APP_NAME"
  local dst_bin="$full_package_macos_dir/${APP_NAME}.app/Contents/MacOS/$APP_NAME"
  local src_hash dst_hash
  src_hash="$(sha256sum "$src_bin" | awk '{print $1}')"
  dst_hash="$(sha256sum "$dst_bin" | awk '{print $1}')"
  [[ "$src_hash" == "$dst_hash" ]] || die "macOS app copy mismatch src=$src_hash dst=$dst_hash"

  run cp -r "$IOS_APP_DIR" "$BUILD_DIR_FULL_PACKAGE/build_ios/${APP_NAME}.app"
  run cp "$IOS_IPA_PATH" "$BUILD_DIR_FULL_PACKAGE/build_ios/"

  log "full package dir: $BUILD_DIR_FULL_PACKAGE"
}

cmd_package() {
  local target="${1:-all}"
  target="$(normalize_platform "$target")" || die "invalid package target: $target"

  log "target: $target"

  case "$target" in
    ios)
      step 1 1 "package ios"
      package_ios_native
      sync_ios_to_full_package
      ;;
    macos)
      step 1 1 "package macos"
      package_macos_native
      sync_macos_to_full_package
      ;;
    windows)
      step 1 1 "package windows"
      package_windows_native
      ;;
    all)
      step 1 1 "package all"
      package_all_native
      ;;
    *)
      die "invalid package target: $target"
      ;;
  esac
}
