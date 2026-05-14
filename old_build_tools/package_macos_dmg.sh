##!/usr/bin/env bash
#set -euo pipefail
#
#APP_NAME="Synthem"
#BUILD_DIR="build_macos_package"
#APP_PATH="$BUILD_DIR/${APP_NAME}.app"
#
#DMG_ROOT="$BUILD_DIR/dmg_root"
#DMG_PATH="$BUILD_DIR/${APP_NAME}.dmg"
#VOLUME_NAME="${APP_NAME}"
#
#if ! command -v hdiutil >/dev/null 2>&1; then
#  echo "error: hdiutil not found. DMG packaging must run on macOS."
#  exit 1
#fi
#
#if [ ! -d "$APP_PATH" ]; then
#  echo "error: missing app bundle: $APP_PATH"
#  echo "run ./package_macos_app.sh first"
#  exit 1
#fi
#
#echo "Packaging ${APP_NAME}.dmg..."
#
#rm -rf "$DMG_ROOT"
#rm -f "$DMG_PATH"
#
#mkdir -p "$DMG_ROOT"
#
#cp -R "$APP_PATH" "$DMG_ROOT/"
#
## Classic drag-to-install shortcut.
#ln -s /Applications "$DMG_ROOT/Applications"
#
#hdiutil create \
#  -volname "$VOLUME_NAME" \
#  -srcfolder "$DMG_ROOT" \
#  -ov \
#  -format UDZO \
#  "$DMG_PATH"
#
#rm -rf "$DMG_ROOT"
#
#echo "Created $DMG_PATH"
#!/usr/bin/env bash
set -euo pipefail

APP_NAME="Synthem"
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"
BUILD_DIR="$BUILD_MACOS_PACKAGE_DIR"
APP_PATH="$BUILD_DIR/${APP_NAME}.app"

IMAGE_PATH="$BUILD_DIR/${APP_NAME}-macOS.hfs"
MOUNT_DIR="$BUILD_DIR/hfs_mount"

rm -f "$IMAGE_PATH"
rm -rf "$MOUNT_DIR"

dd if=/dev/zero of="$IMAGE_PATH" bs=1M count=250
mkfs.hfsplus -v "$APP_NAME" "$IMAGE_PATH"

mkdir -p "$MOUNT_DIR"

sudo mount -o loop,rw -t hfsplus "$IMAGE_PATH" "$MOUNT_DIR"

sudo cp -R "$APP_PATH" "$MOUNT_DIR/"
sudo ln -s /Applications "$MOUNT_DIR/Applications"

sync
sudo umount "$MOUNT_DIR"
rm -rf "$MOUNT_DIR"

echo "Created $IMAGE_PATH"
