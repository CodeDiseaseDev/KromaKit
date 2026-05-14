#!/usr/bin/env bash
set -euo pipefail

_render_png() {
  local input_svg="$1"
  local size="$2"
  local output="$3"
  run rsvg-convert -w "$size" -h "$size" "$input_svg" -o "$output"
}

_make_themed_icon_svg() {
  local input_svg="$1"
  local output_svg="$2"
  local top_colour="$3"
  local bottom_colour="$4"

  [[ -f "$input_svg" ]] || die "missing svg icon source: $input_svg"

  [[ "$top_colour" =~ ^#[0-9A-Fa-f]{6}$ ]] ||
    die "invalid top icon colour: $top_colour"

  [[ "$bottom_colour" =~ ^#[0-9A-Fa-f]{6}$ ]] ||
    die "invalid bottom icon colour: $bottom_colour"

  sed \
    -e "s/#7F7F7F/$top_colour/g" \
    -e "s/#4F4F4F/$bottom_colour/g" \
    "$input_svg" > "$output_svg"
}

_resolve_icon_theme() {
  local theme="$1"

  case "$theme" in
    blue|default|ios)
      ICON_TOP_COLOUR="#36C7FF"
      ICON_BOTTOM_COLOUR="#0075C9"
      ;;

    coral|red|synthem)
      ICON_TOP_COLOUR="#FF746A"
      ICON_BOTTOM_COLOUR="#FF3B30"
      ;;

    graphite|mono|gray|grey)
      ICON_TOP_COLOUR="#7F7F7F"
      ICON_BOTTOM_COLOUR="#4F4F4F"
      ;;

    midnight|night)
      ICON_TOP_COLOUR="#30364F"
      ICON_BOTTOM_COLOUR="#10131F"
      ;;

    void|black)
      ICON_TOP_COLOUR="#2A2A2E"
      ICON_BOTTOM_COLOUR="#050506"
      ;;

    ice|cyan)
      ICON_TOP_COLOUR="#9BE8FF"
      ICON_BOTTOM_COLOUR="#1DA7E8"
      ;;

    aqua|teal)
      ICON_TOP_COLOUR="#4DFFE1"
      ICON_BOTTOM_COLOUR="#0087A8"
      ;;

    mint|green)
      ICON_TOP_COLOUR="#7CFFB2"
      ICON_BOTTOM_COLOUR="#00A86B"
      ;;

    lime|acid)
      ICON_TOP_COLOUR="#D9FF57"
      ICON_BOTTOM_COLOUR="#52B000"
      ;;

    purple|violet)
      ICON_TOP_COLOUR="#B66DFF"
      ICON_BOTTOM_COLOUR="#5B2EFF"
      ;;

    grape)
      ICON_TOP_COLOUR="#D58CFF"
      ICON_BOTTOM_COLOUR="#6A00B9"
      ;;

    pink|bubblegum)
      ICON_TOP_COLOUR="#FF8AD8"
      ICON_BOTTOM_COLOUR="#FF2D8F"
      ;;

    hotpink|neonpink)
      ICON_TOP_COLOUR="#FF5AF7"
      ICON_BOTTOM_COLOUR="#C400FF"
      ;;

    sunset|orange)
      ICON_TOP_COLOUR="#FFB45C"
      ICON_BOTTOM_COLOUR="#FF4B2B"
      ;;

    amber|gold)
      ICON_TOP_COLOUR="#FFD36A"
      ICON_BOTTOM_COLOUR="#FF9F0A"
      ;;

    lava|fire)
      ICON_TOP_COLOUR="#FF7A3D"
      ICON_BOTTOM_COLOUR="#B40020"
      ;;

    candy)
      ICON_TOP_COLOUR="#FF7AC8"
      ICON_BOTTOM_COLOUR="#5AC8FA"
      ;;

    aurora)
      ICON_TOP_COLOUR="#6EFFC5"
      ICON_BOTTOM_COLOUR="#5E5CFF"
      ;;

    synthwave|retro)
      ICON_TOP_COLOUR="#FF4FD8"
      ICON_BOTTOM_COLOUR="#261BFF"
      ;;

    vaporwave)
      ICON_TOP_COLOUR="#FF9DEB"
      ICON_BOTTOM_COLOUR="#6AE4FF"
      ;;

    cyber)
      ICON_TOP_COLOUR="#00FFF0"
      ICON_BOTTOM_COLOUR="#7A00FF"
      ;;

    matrix)
      ICON_TOP_COLOUR="#7CFF6B"
      ICON_BOTTOM_COLOUR="#003B16"
      ;;

    warning|hazard)
      ICON_TOP_COLOUR="#FFD60A"
      ICON_BOTTOM_COLOUR="#FF453A"
      ;;

    dev)
      ICON_TOP_COLOUR="#FFCC00"
      ICON_BOTTOM_COLOUR="#FF3B30"
      ;;

    beta)
      ICON_TOP_COLOUR="#C58BFF"
      ICON_BOTTOM_COLOUR="#5B2EFF"
      ;;

    debug)
      ICON_TOP_COLOUR="#00E5FF"
      ICON_BOTTOM_COLOUR="#0057FF"
      ;;

    nightly)
      ICON_TOP_COLOUR="#5E5CE6"
      ICON_BOTTOM_COLOUR="#1C1C1E"
      ;;

    trollstore|troll)
      ICON_TOP_COLOUR="#A6FF00"
      ICON_BOTTOM_COLOUR="#00C7BE"
      ;;

    coffee)
      ICON_TOP_COLOUR="#C88A5A"
      ICON_BOTTOM_COLOUR="#4A2A18"
      ;;

    monster)
      ICON_TOP_COLOUR="#B8FF00"
      ICON_BOTTOM_COLOUR="#008F39"
      ;;

    finalcut|fcp)
      ICON_TOP_COLOUR="#772780"
      ICON_BOTTOM_COLOUR="#2a035c"
      ;;

    light|day)
      ICON_TOP_COLOUR="#FFFFFF"
      ICON_BOTTOM_COLOUR="#CCCCCC"
      ;;

    deepblack)
      ICON_TOP_COLOUR="#000000"
      ICON_BOTTOM_COLOUR="#000000"
      ;;

    mica|micablack)
      ICON_TOP_COLOUR="#3A3D46"
      ICON_BOTTOM_COLOUR="#0E1014"
      ;;

    *)
      die "invalid icon theme: $theme (expected blue|coral|graphite|midnight|ice|aqua|mint|purple|pink|sunset|amber|lava|aurora|synthwave|vaporwave|cyber|dev|beta|debug|nightly|etc)"
      ;;
  esac
}

generate_windows_icons() {
  local input_svg="${1:-$ICON_SVG_PATH}"
  local out_dir="${2:-$BUILD_DIR_ICONS}"
  local win_dir="$out_dir/windows"
  local tmp_dir="$win_dir/tmp"
  local ico_path="$win_dir/${APP_NAME}.ico"

  [[ -f "$input_svg" ]] || die "missing svg icon source: $input_svg"
  command -v rsvg-convert >/dev/null 2>&1 || die "missing required tool: rsvg-convert"
  command -v magick >/dev/null 2>&1 || die "missing required tool: magick"

  run rm -rf "$win_dir"
  run mkdir -p "$tmp_dir"

  local sizes="16 24 32 48 64 128 256"
  local pngs=()

  local size
  for size in $sizes; do
    local png="$tmp_dir/icon_${size}.png"
    _render_png "$input_svg" "$size" "$png"
    pngs+=("$png")
  done

  run magick "${pngs[@]}" "$ico_path"

  run rm -rf "$tmp_dir"
  [[ -f "$ico_path" ]] || die "failed to create Windows icon: $ico_path"

  log "Windows icon ready: $ico_path"
}

# shellcheck disable=SC2120
generate_ios_icons() {
  local input_svg="${1:-$ICON_SVG_PATH}"
  local out_dir="${2:-$BUILD_DIR_ICONS}"
  local png_dir="$out_dir/png"
  local ios_dir="$out_dir/ios/AppIcon.appiconset"

  [[ -f "$input_svg" ]] || die "missing svg icon source: $input_svg"
  command -v rsvg-convert >/dev/null 2>&1 || die "missing required tool: rsvg-convert"

  run mkdir -p "$png_dir" "$ios_dir"

  local sizes="16 20 29 32 40 48 58 60 64 76 80 87 120 128 152 167 180 256 512 1024"
  local size
  for size in $sizes; do
    _render_png "$input_svg" "$size" "$png_dir/icon_${size}.png"
  done

  _render_png "$input_svg" 40 "$ios_dir/Icon-20@2x.png"
  _render_png "$input_svg" 60 "$ios_dir/Icon-20@3x.png"
  _render_png "$input_svg" 58 "$ios_dir/Icon-29@2x.png"
  _render_png "$input_svg" 87 "$ios_dir/Icon-29@3x.png"
  _render_png "$input_svg" 80 "$ios_dir/Icon-40@2x.png"
  _render_png "$input_svg" 120 "$ios_dir/Icon-40@3x.png"
  _render_png "$input_svg" 120 "$ios_dir/Icon-60@2x.png"
  _render_png "$input_svg" 180 "$ios_dir/Icon-60@3x.png"
  _render_png "$input_svg" 20 "$ios_dir/Icon-20@1x.png"
  _render_png "$input_svg" 29 "$ios_dir/Icon-29@1x.png"
  _render_png "$input_svg" 40 "$ios_dir/Icon-40@1x.png"
  _render_png "$input_svg" 76 "$ios_dir/Icon-76@1x.png"
  _render_png "$input_svg" 152 "$ios_dir/Icon-76@2x.png"
  _render_png "$input_svg" 167 "$ios_dir/Icon-83.5@2x.png"
  _render_png "$input_svg" 1024 "$ios_dir/Icon-1024.png"

  cat > "$ios_dir/Contents.json" <<'JSON'
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

  log "iOS app icons ready: $ios_dir"
}

generate_macos_icons() {
  local input_svg="${1:-$ICON_SVG_PATH}"
  local out_dir="${2:-$BUILD_DIR_ICONS}"
  local mac_dir="$out_dir/macos"
  local mac_iconset="$mac_dir/Synthem.iconset"
  local mac_icns="$mac_dir/Synthem.icns"
  local tmp_dir="$mac_dir/tmp"
  local raw_1024="$tmp_dir/raw_icon_1024.png"
  local tile="$tmp_dir/tile_1024.png"
  local mask="$tmp_dir/mask_1024.png"
  local master="$mac_dir/Synthem_macos_1024.png"

  [[ -f "$input_svg" ]] || die "missing svg icon source: $input_svg"
  command -v rsvg-convert >/dev/null 2>&1 || die "missing required tool: rsvg-convert"
  command -v magick >/dev/null 2>&1 || die "missing required tool: magick"
  if ! command -v png2icns >/dev/null 2>&1 && ! command -v iconutil >/dev/null 2>&1; then
    die "missing required tool: png2icns or iconutil"
  fi

  run rm -rf "$mac_dir"
  run mkdir -p "$mac_iconset" "$tmp_dir"

  local canvas=1024
  local tile_size=864
  local tile_offset=80
  local corner_radius=190

  _render_png "$input_svg" "$canvas" "$raw_1024"
  run magick "$raw_1024" -resize "${tile_size}x${tile_size}" "$tile"
  run magick -size "${tile_size}x${tile_size}" xc:none -fill white \
    -draw "roundrectangle 0,0 $((tile_size - 1)),$((tile_size - 1)) ${corner_radius},${corner_radius}" \
    "$mask"
  run magick "$tile" "$mask" -alpha set -compose DstIn -composite "$tile"
  run magick -size "${canvas}x${canvas}" xc:none "$tile" -geometry "+${tile_offset}+${tile_offset}" -composite "$master"

  run magick "$master" -resize 16x16 "$mac_iconset/icon_16x16.png"
  run magick "$master" -resize 32x32 "$mac_iconset/icon_16x16@2x.png"
  run magick "$master" -resize 32x32 "$mac_iconset/icon_32x32.png"
  run magick "$master" -resize 64x64 "$mac_iconset/icon_32x32@2x.png"
  run magick "$master" -resize 128x128 "$mac_iconset/icon_128x128.png"
  run magick "$master" -resize 256x256 "$mac_iconset/icon_128x128@2x.png"
  run magick "$master" -resize 256x256 "$mac_iconset/icon_256x256.png"
  run magick "$master" -resize 512x512 "$mac_iconset/icon_256x256@2x.png"
  run magick "$master" -resize 512x512 "$mac_iconset/icon_512x512.png"
  run magick "$master" -resize 1024x1024 "$mac_iconset/icon_512x512@2x.png"

  run rm -f "$mac_icns"

  local icns_packer="/home/code/Coding/test_ios_app/scripts/other_scripts/icns_generator.py"

  [[ -f "$icns_packer" ]] || die "missing ICNS packer: $icns_packer"
  command -v python3 >/dev/null 2>&1 || die "missing required tool: python3"

  run python3 "$icns_packer" "$mac_iconset" "$mac_icns"

  run rm -rf "$tmp_dir"
  [[ -f "$mac_icns" ]] || die "failed to create macOS icon set: $mac_icns"
  log "macOS icons ready: $mac_icns"
}

cmd_icons() {
  log "cmd_icons args: $*"

  local target="${1:-all}"
  shift || true

  local theme="${OVERRIDE_THEME:-night}"

  while [[ $# -gt 0 ]]; do
    case "$1" in
      --theme)
        [[ $# -ge 2 ]] || die "missing value for --theme"
        theme="$2"
        shift 2
        ;;
      *)
        die "unknown icons flag: $1"
        ;;
    esac
  done

  _resolve_icon_theme "$theme"

  local themed_svg="$BUILD_DIR_ICONS/AppIcon.${theme}.generated.svg"
  run mkdir -p "$BUILD_DIR_ICONS"

  _make_themed_icon_svg \
    "$ICON_SVG_PATH" \
    "$themed_svg" \
    "$ICON_TOP_COLOUR" \
    "$ICON_BOTTOM_COLOUR"

  case "$target" in
    ios)
      log "target: ios"
      log "icon theme: $theme"
      generate_ios_icons "$themed_svg"
      ;;
    macos)
      log "target: macos"
      log "icon theme: $theme"
      generate_macos_icons "$themed_svg"
      ;;
    windows)
      log "target: windows"
      log "icon theme: $theme"
      generate_windows_icons "$themed_svg"
      ;;
    all)
      log "target: all"
      log "icon theme: $theme"
      generate_macos_icons "$themed_svg"
      generate_ios_icons "$themed_svg"
      generate_windows_icons "$themed_svg"
      ;;
    *)
      die "invalid icon target: $target (expected ios|macos|all)"
      ;;
  esac

  rm $themed_svg
}
