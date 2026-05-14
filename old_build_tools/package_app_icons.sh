#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"

INPUT_SVG="${1:-resources/icon.svg}"
OUT_DIR="${2:-$BUILD_ICONS_DIR}"

MAC_DIR="$OUT_DIR/macos"
MAC_ICONSET="$MAC_DIR/Synthem.iconset"
MAC_ICNS="$MAC_DIR/Synthem.icns"

MAC_MASTER="$MAC_DIR/Synthem_macos_1024.png"

TMP_DIR="$MAC_DIR/tmp"
TMP_RAW_1024="$TMP_DIR/raw_icon_1024.png"
TMP_TILE="$TMP_DIR/tile_1024.png"
TMP_MASK="$TMP_DIR/mask_1024.png"

# macOS-style tile settings.
# Final canvas stays 1024x1024 with transparent outside corners.
MAC_CANVAS_SIZE=1024
MAC_TILE_OFFSET=80
MAC_TILE_SIZE=864
MAC_CORNER_RADIUS=190

if ! command -v rsvg-convert >/dev/null 2>&1; then
  echo "error: rsvg-convert not found. Install librsvg."
  exit 1
fi

if ! command -v magick >/dev/null 2>&1; then
  echo "error: magick not found. Install imagemagick."
  exit 1
fi

if ! command -v png2icns >/dev/null 2>&1 && ! command -v iconutil >/dev/null 2>&1; then
  echo "error: neither png2icns nor iconutil were found."
  echo "on Arch, install icnsutils."
  echo "on macOS, iconutil should already exist."
  exit 1
fi

if [ ! -f "$INPUT_SVG" ]; then
  echo "error: missing svg: $INPUT_SVG"
  exit 1
fi

rm -rf "$MAC_DIR"
mkdir -p "$MAC_ICONSET" "$TMP_DIR"

echo "Rendering raw 1024x1024 SVG..."

rsvg-convert \
  -w "$MAC_CANVAS_SIZE" \
  -h "$MAC_CANVAS_SIZE" \
  "$INPUT_SVG" \
  -o "$TMP_RAW_1024"

echo "Creating macOS rounded transparent icon master..."

# Resize the raw square icon into the inset macOS tile size.
magick "$TMP_RAW_1024" \
  -resize "${MAC_TILE_SIZE}x${MAC_TILE_SIZE}" \
  "$TMP_TILE"

# Create rounded alpha mask.
magick \
  -size "${MAC_TILE_SIZE}x${MAC_TILE_SIZE}" \
  xc:none \
  -fill white \
  -draw "roundrectangle 0,0 $((MAC_TILE_SIZE - 1)),$((MAC_TILE_SIZE - 1)) ${MAC_CORNER_RADIUS},${MAC_CORNER_RADIUS}" \
  "$TMP_MASK"

# Apply rounded alpha mask to the tile.
magick "$TMP_TILE" "$TMP_MASK" \
  -alpha set \
  -compose DstIn \
  -composite \
  "$TMP_TILE"

# Place rounded tile onto transparent 1024x1024 canvas.
magick \
  -size "${MAC_CANVAS_SIZE}x${MAC_CANVAS_SIZE}" \
  xc:none \
  "$TMP_TILE" \
  -geometry "+${MAC_TILE_OFFSET}+${MAC_TILE_OFFSET}" \
  -composite \
  "$MAC_MASTER"

render_macos_png() {
  local size="$1"
  local output="$2"

  magick "$MAC_MASTER" \
    -resize "${size}x${size}" \
    "$output"
}

echo "Rendering macOS iconset..."

render_macos_png 16   "$MAC_ICONSET/icon_16x16.png"
render_macos_png 32   "$MAC_ICONSET/icon_16x16@2x.png"
render_macos_png 32   "$MAC_ICONSET/icon_32x32.png"
render_macos_png 64   "$MAC_ICONSET/icon_32x32@2x.png"
render_macos_png 128  "$MAC_ICONSET/icon_128x128.png"
render_macos_png 256  "$MAC_ICONSET/icon_128x128@2x.png"
render_macos_png 256  "$MAC_ICONSET/icon_256x256.png"
render_macos_png 512  "$MAC_ICONSET/icon_256x256@2x.png"
render_macos_png 512  "$MAC_ICONSET/icon_512x512.png"
render_macos_png 1024 "$MAC_ICONSET/icon_512x512@2x.png"

echo "Creating .icns..."

rm -f "$MAC_ICNS"

if command -v iconutil >/dev/null 2>&1; then
  iconutil \
    -c icns \
    "$MAC_ICONSET" \
    -o "$MAC_ICNS"
else
  # Linux/Arch path. png2icns only wants one file per actual pixel size.
  png2icns "$MAC_ICNS" \
    "$MAC_ICONSET/icon_16x16.png" \
    "$MAC_ICONSET/icon_32x32.png" \
    "$MAC_ICONSET/icon_128x128.png" \
    "$MAC_ICONSET/icon_256x256.png" \
    "$MAC_ICONSET/icon_512x512.png"
fi

if [ ! -f "$MAC_ICNS" ]; then
  echo "error: failed to create macOS icns: $MAC_ICNS"
  exit 1
fi

rm -rf "$TMP_DIR"

echo
echo "Done."
echo "macOS master PNG: $MAC_MASTER"
echo "macOS iconset:    $MAC_ICONSET"
echo "macOS icns:       $MAC_ICNS"

#!/usr/bin/env bash
set -euo pipefail

INPUT_SVG="${1:-resources/icon.svg}"
OUT_DIR="${2:-$BUILD_ICONS_DIR}"

PNG_DIR="$OUT_DIR/png"
IOS_DIR="$OUT_DIR/ios/AppIcon.appiconset"
MAC_ICONSET="$OUT_DIR/macos/Synthem.iconset"
MAC_ICNS="$OUT_DIR/macos/Synthem.icns"

if ! command -v rsvg-convert >/dev/null 2>&1; then
  echo "error: rsvg-convert not found. Install librsvg."
  exit 1
fi

if ! command -v png2icns >/dev/null 2>&1 && ! command -v iconutil >/dev/null 2>&1; then
  echo "error: neither png2icns nor iconutil were found."
  echo "on Arch, install icnsutils."
  echo "on macOS, iconutil should already exist."
  exit 1
fi

if [ ! -f "$INPUT_SVG" ]; then
  echo "error: missing svg: $INPUT_SVG"
  exit 1
fi

rm -rf "$OUT_DIR"
mkdir -p "$PNG_DIR" "$IOS_DIR" "$MAC_ICONSET" "$(dirname "$MAC_ICNS")"

render_png() {
  local size="$1"
  local output="$2"

  rsvg-convert \
    -w "$size" \
    -h "$size" \
    "$INPUT_SVG" \
    -o "$output"
}

echo "Rendering generic PNGs..."

for size in 16 20 29 32 40 48 58 60 64 76 80 87 120 128 152 167 180 256 512 1024; do
  render_png "$size" "$PNG_DIR/icon_${size}.png"
done

echo "Rendering iOS AppIcon.appiconset..."

# iPhone notification
render_png 40  "$IOS_DIR/Icon-20@2x.png"
render_png 60  "$IOS_DIR/Icon-20@3x.png"

# iPhone settings
render_png 58  "$IOS_DIR/Icon-29@2x.png"
render_png 87  "$IOS_DIR/Icon-29@3x.png"

# iPhone spotlight
render_png 80  "$IOS_DIR/Icon-40@2x.png"
render_png 120 "$IOS_DIR/Icon-40@3x.png"

# iPhone app
render_png 120 "$IOS_DIR/Icon-60@2x.png"
render_png 180 "$IOS_DIR/Icon-60@3x.png"

# iPad notification/settings/spotlight/app
render_png 20  "$IOS_DIR/Icon-20@1x.png"
render_png 29  "$IOS_DIR/Icon-29@1x.png"
render_png 40  "$IOS_DIR/Icon-40@1x.png"
render_png 76  "$IOS_DIR/Icon-76@1x.png"
render_png 152 "$IOS_DIR/Icon-76@2x.png"
render_png 167 "$IOS_DIR/Icon-83.5@2x.png"

# App Store
render_png 1024 "$IOS_DIR/Icon-1024.png"

cat > "$IOS_DIR/Contents.json" <<'JSON'
{
  "images": [
    { "idiom": "iphone", "size": "20x20", "scale": "2x", "filename": "Icon-20@2x.png" },
    { "idiom": "iphone", "size": "20x20", "scale": "3x", "filename": "Icon-20@3x.png" },

    { "idiom": "iphone", "size": "29x29", "scale": "2x", "filename": "Icon-29@2x.png" },
    { "idiom": "iphone", "size": "29x29", "scale": "3x", "filename": "Icon-29@3x.png" },

    { "idiom": "iphone", "size": "40x40", "scale": "2x", "filename": "Icon-40@2x.png" },
    { "idiom": "iphone", "size": "40x40", "scale": "3x", "filename": "Icon-40@3x.png" },

    { "idiom": "iphone", "size": "60x60", "scale": "2x", "filename": "Icon-60@2x.png" },
    { "idiom": "iphone", "size": "60x60", "scale": "3x", "filename": "Icon-60@3x.png" },

    { "idiom": "ipad", "size": "20x20", "scale": "1x", "filename": "Icon-20@1x.png" },
    { "idiom": "ipad", "size": "20x20", "scale": "2x", "filename": "Icon-20@2x.png" },

    { "idiom": "ipad", "size": "29x29", "scale": "1x", "filename": "Icon-29@1x.png" },
    { "idiom": "ipad", "size": "29x29", "scale": "2x", "filename": "Icon-29@2x.png" },

    { "idiom": "ipad", "size": "40x40", "scale": "1x", "filename": "Icon-40@1x.png" },
    { "idiom": "ipad", "size": "40x40", "scale": "2x", "filename": "Icon-40@2x.png" },

    { "idiom": "ipad", "size": "76x76", "scale": "1x", "filename": "Icon-76@1x.png" },
    { "idiom": "ipad", "size": "76x76", "scale": "2x", "filename": "Icon-76@2x.png" },

    { "idiom": "ipad", "size": "83.5x83.5", "scale": "2x", "filename": "Icon-83.5@2x.png" },

    { "idiom": "ios-marketing", "size": "1024x1024", "scale": "1x", "filename": "Icon-1024.png" }
  ],
  "info": {
    "author": "xcode",
    "version": 1
  }
}
JSON

#echo "Rendering macOS iconset..."

#render_png 16   "$MAC_ICONSET/icon_16x16.png"
#render_png 32   "$MAC_ICONSET/icon_16x16@2x.png"
#render_png 32   "$MAC_ICONSET/icon_32x32.png"
#render_png 64   "$MAC_ICONSET/icon_32x32@2x.png"
#render_png 128  "$MAC_ICONSET/icon_128x128.png"
#render_png 256  "$MAC_ICONSET/icon_128x128@2x.png"
#render_png 256  "$MAC_ICONSET/icon_256x256.png"
#render_png 512  "$MAC_ICONSET/icon_256x256@2x.png"
#render_png 512  "$MAC_ICONSET/icon_512x512.png"
#render_png 1024 "$MAC_ICONSET/icon_512x512@2x.png"

#echo "Creating .icns..."
#
#rm -f "$MAC_ICNS"
#
#if command -v iconutil >/dev/null 2>&1; then
#  # macOS-native path. iconutil understands .iconset naming and @2x variants.
#  iconutil \
#    -c icns \
#    "$MAC_ICONSET" \
#    -o "$MAC_ICNS"
#else
#  # Linux/Arch path. png2icns only wants one file per actual pixel size.
#  # Do not pass @2x files here, because e.g. 16@2x and 32x32 are both 32px
#  # and png2icns treats that as a duplicate icon element.
#  png2icns "$MAC_ICNS" \
#    "$MAC_ICONSET/icon_16x16.png" \
#    "$MAC_ICONSET/icon_32x32.png" \
#    "$MAC_ICONSET/icon_128x128.png" \
#    "$MAC_ICONSET/icon_256x256.png" \
#    "$MAC_ICONSET/icon_512x512.png"
#fi
#
#if [ ! -f "$MAC_ICNS" ]; then
#  echo "error: failed to create macOS icns: $MAC_ICNS"
#  exit 1
#fi

echo
echo "Done."
echo "PNG icons:       $PNG_DIR"
echo "iOS appiconset:  $IOS_DIR"
