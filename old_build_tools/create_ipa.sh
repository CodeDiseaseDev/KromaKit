#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"

(
  cd "$BUILD_IOS_PACKAGE_DIR"

  rm -rf Payload Synthem.ipa

  mkdir -p Payload/Synthem.app

  cp Synthem.rawexec Payload/Synthem.app/Synthem

  (
    cd ..

    # Copy normal resources, but avoid the old manually-maintained icon.png.
      rsync -a \
      --exclude='icon.png' \
      resources/ \
      "$BUILD_IOS_PACKAGE_DIR/Payload/Synthem.app/"

    # Copy generated iOS app icons into the .app bundle.
    ICONSET_DIR="$BUILD_ICONS_DIR/ios/AppIcon.appiconset"

    if [ ! -d "$ICONSET_DIR" ]; then
      echo "error: missing generated iOS icon set: $ICONSET_DIR"
      echo "run ./generate_icons.sh first"
      exit 1
    fi

    cp "$ICONSET_DIR"/*.png "$BUILD_IOS_PACKAGE_DIR/Payload/Synthem.app/"

    ./generate_info.plist.sh "$BUILD_IOS_PACKAGE_DIR/Payload/Synthem.app/Info.plist"
  )

  /home/code/ldid/ldid -S Payload/Synthem.app/Synthem
  /home/code/ldid/ldid -S Payload/Synthem.app

  zip -r Synthem.ipa Payload > /dev/null || {
    exit 1
  }
)
