#!/usr/bin/env bash
set -euo pipefail

APP_NAME="Synthem"
BUNDLE_ID="com.codelmao.synthem"
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"
BUILD_DIR="$BUILD_MACOS_DIR"
PACKAGE_DIR="$BUILD_MACOS_PACKAGE_DIR"
APP_DIR="$PACKAGE_DIR/$APP_NAME.app"
PROC_BIN_DIR="/home/code/procursus/cctools-port/usage_examples/macos_toolchain/target/bin"
LDID_BIN="${LDID_BIN:-$PROC_BIN_DIR/ldid}"

RAW_EXEC="$BUILD_DIR/Synthem"

ICNS_PATH="$BUILD_ICONS_DIR/macos/Synthem.icns"

rm -rf "$PACKAGE_DIR"
mkdir -p "$APP_DIR/Contents/MacOS"
mkdir -p "$APP_DIR/Contents/Resources"

cp "$RAW_EXEC" "$APP_DIR/Contents/MacOS/$APP_NAME"
chmod +x "$APP_DIR/Contents/MacOS/$APP_NAME"

# Normalize signature state on the main Mach-O with Procursus ldid.
# Use only flags supported by the Procursus ldid build in this repo.
if [ ! -x "$LDID_BIN" ]; then
  echo "error: ldid not found at: $LDID_BIN"
  exit 1
fi

"$LDID_BIN" -S "$APP_DIR/Contents/MacOS/$APP_NAME"
echo "ldid signature digest:"
"$LDID_BIN" -d "$APP_DIR/Contents/MacOS/$APP_NAME"

if [ -d "resources" ]; then
  rsync -a \
    --exclude='icon.png' \
    resources/ \
    "$APP_DIR/Contents/Resources/"
fi

if [ ! -f "$ICNS_PATH" ]; then
  echo "error: missing macOS icon: $ICNS_PATH"
  echo "run ./package_app_icons.sh first"
  exit 1
fi

cp "$ICNS_PATH" "$APP_DIR/Contents/Resources/Synthem.icns"

cat > "$APP_DIR/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
 "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleName</key>
  <string>$APP_NAME</string>

  <key>CFBundleDisplayName</key>
  <string>$APP_NAME</string>

  <key>CFBundleIdentifier</key>
  <string>$BUNDLE_ID</string>

  <key>CFBundleVersion</key>
  <string>1</string>

  <key>CFBundleShortVersionString</key>
  <string>0.1.0</string>

  <key>CFBundleExecutable</key>
  <string>$APP_NAME</string>

  <key>CFBundleIconFile</key>
  <string>Synthem</string>

  <key>CFBundlePackageType</key>
  <string>APPL</string>

  <key>CFBundleSignature</key>
  <string>????</string>

  <key>LSMinimumSystemVersion</key>
  <string>10.14</string>

  <key>NSHighResolutionCapable</key>
  <true/>

  <key>NSSupportsAutomaticGraphicsSwitching</key>
  <true/>

  <key>CFBundleIconFile</key>
  <string>Synthem.icns</string>
</dict>
</plist>
EOF

# If packaging runs on macOS, re-sign the completed bundle so the
# resource envelope matches the final copied files.
if [ "$(uname -s)" = "Darwin" ] && command -v codesign >/dev/null 2>&1; then
  echo "Applying ad-hoc macOS bundle signature..."
  codesign --force --deep --sign - "$APP_DIR"
  codesign --verify --deep --strict --verbose=2 "$APP_DIR"
fi

echo "Built macOS app bundle at $APP_DIR"
