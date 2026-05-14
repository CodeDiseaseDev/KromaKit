#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"

#package iOS .ipa
./create_ipa.sh

#package macOS .app
./package_macos_app.sh
#./package_macos_dmg.sh

rm -rf "$BUILD_FULL_PACKAGE_DIR"
mkdir -p \
  "$BUILD_FULL_PACKAGE_DIR/$BUILD_LINUX_DIR/resources" \
  "$BUILD_FULL_PACKAGE_DIR/$BUILD_WINDOWS_DIR/resources" \
  "$BUILD_FULL_PACKAGE_DIR/$BUILD_MACOS_DIR" \
  "$BUILD_FULL_PACKAGE_DIR/build_ios"

rsync -a --exclude='icon.png' resources/* "$BUILD_FULL_PACKAGE_DIR/$BUILD_LINUX_DIR/resources"
rsync -a --exclude='icon.png' resources/* "$BUILD_FULL_PACKAGE_DIR/$BUILD_WINDOWS_DIR/resources"

rsync -a --exclude='obj/' "$BUILD_LINUX_DIR/" "$BUILD_FULL_PACKAGE_DIR/$BUILD_LINUX_DIR/"
rsync -a --exclude='obj/' "$BUILD_WINDOWS_DIR/" "$BUILD_FULL_PACKAGE_DIR/$BUILD_WINDOWS_DIR/"

cp -r "$BUILD_MACOS_PACKAGE_DIR/Synthem.app" "$BUILD_FULL_PACKAGE_DIR/$BUILD_MACOS_DIR/Synthem.app"

MACOS_SRC_BIN="$BUILD_MACOS_PACKAGE_DIR/Synthem.app/Contents/MacOS/Synthem"
MACOS_DST_BIN="$BUILD_FULL_PACKAGE_DIR/$BUILD_MACOS_DIR/Synthem.app/Contents/MacOS/Synthem"
SRC_HASH="$(sha256sum "$MACOS_SRC_BIN" | awk '{print $1}')"
DST_HASH="$(sha256sum "$MACOS_DST_BIN" | awk '{print $1}')"
if [ "$SRC_HASH" != "$DST_HASH" ]; then
  echo "error: macOS app copy mismatch (source and packaged hashes differ)"
  echo "src=$SRC_HASH"
  echo "dst=$DST_HASH"
  exit 1
fi

cp -r "$BUILD_IOS_PACKAGE_DIR/Payload/Synthem.app" "$BUILD_FULL_PACKAGE_DIR/build_ios/Synthem.app"
cp -r "$BUILD_IOS_PACKAGE_DIR/Synthem.ipa" "$BUILD_FULL_PACKAGE_DIR/build_ios/"
