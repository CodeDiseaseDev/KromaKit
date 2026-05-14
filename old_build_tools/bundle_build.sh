#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"

PACKAGE_DIR="$BUILD_FULL_PACKAGE_DIR"
TARGET="${1:-all}"

case "$TARGET" in
  all)
    ZIP_NAME="Synthem_full_package.zip"
    INCLUDE_PATH="."
    ;;

  macos)
    ZIP_NAME="Synthem_macos_package.zip"
    INCLUDE_PATH="$BUILD_MACOS_PACKAGE_DIR"
    ;;

  ios)
    ZIP_NAME="Synthem_ios_package.zip"
    INCLUDE_PATH="build_ios"
    ;;

  linux)
    ZIP_NAME="Synthem_linux_package.zip"
    INCLUDE_PATH="$BUILD_LINUX_DIR"
    ;;

  windows)
    ZIP_NAME="Synthem_windows_package.zip"
    INCLUDE_PATH="$BUILD_WINDOWS_DIR"
    ;;

  *)
    echo "error: unknown package target: $TARGET"
    echo "usage: $0 [all|macos|ios|linux|windows]"
    exit 1
    ;;
esac

ZIP_PATH="$PACKAGE_DIR/$ZIP_NAME"

if [ "$TARGET" = "macos" ]; then
  if [ ! -d "$INCLUDE_PATH/Synthem.app" ]; then
    echo "error: missing package content: $INCLUDE_PATH/Synthem.app"
    echo "run ./package_macos_app.sh first"
    exit 1
  fi
  rm -f "$ZIP_PATH"
  zip -r "$ZIP_PATH" "$INCLUDE_PATH"
  echo "Created $ZIP_PATH"
  exit 0
fi

if [ ! -d "$PACKAGE_DIR" ]; then
  echo "error: missing directory: $PACKAGE_DIR"
  exit 1
fi

if [ ! -e "$PACKAGE_DIR/$INCLUDE_PATH" ]; then
  echo "error: missing package content: $PACKAGE_DIR/$INCLUDE_PATH"
  exit 1
fi

rm -f "$ZIP_PATH"

(
  cd "$PACKAGE_DIR"

  zip -r "$ZIP_NAME" "$INCLUDE_PATH" \
    -x "$ZIP_NAME"
)

echo "Created $ZIP_PATH"
