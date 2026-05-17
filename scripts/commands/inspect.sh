#!/usr/bin/env bash

inspect_cmd_exists() {
  command -v "$1" >/dev/null 2>&1
}

inspect_hr() {
  printf '%s\n' "${C_DARK:-}────────────────────────────────────────────────────────────${C_RESET:-}"
}

inspect_title() {
  printf '\n%s%s%s\n' "${C_BOLD:-}${C_PURPLE:-}" "$1" "${C_RESET:-}"
  inspect_hr
}

inspect_section() {
  printf '\n%s%s%s\n' "${C_BOLD:-}${C_CYAN:-}" "$1" "${C_RESET:-}"
}

inspect_kv() {
  local key="$1"
  local value="${2:-}"
  printf '  %s%-24s%s %s\n' "${C_GRAY:-}" "$key" "${C_RESET:-}" "$value"
}

inspect_ok_kv() {
  local key="$1"
  local value="${2:-}"
  printf '  %s✓%s %-22s %s\n' "${C_GREEN:-}" "${C_RESET:-}" "$key" "$value"
}

inspect_warn_kv() {
  local key="$1"
  local value="${2:-}"
  printf '  %s!%s %-22s %s\n' "${C_YELLOW:-}" "${C_RESET:-}" "$key" "$value"
}

inspect_fail_kv() {
  local key="$1"
  local value="${2:-}"
  printf '  %s✗%s %-22s %s\n' "${C_RED:-}" "${C_RESET:-}" "$key" "$value"
}

inspect_path() {
  local label="$1"
  local path="$2"

  if [[ -e "$path" ]]; then
    inspect_ok_kv "$label" "$path"
  else
    inspect_fail_kv "$label" "missing: $path"
  fi
}

inspect_file() {
  local path="$1"

  [[ -e "$path" ]] || {
    inspect_fail_kv "file" "missing: $path"
    return 0
  }

  inspect_kv "path" "$path"

  if [[ -f "$path" ]]; then
    inspect_kv "kind" "regular file"
  elif [[ -d "$path" ]]; then
    inspect_kv "kind" "directory"
  elif [[ -L "$path" ]]; then
    inspect_kv "kind" "symlink -> $(readlink "$path")"
  else
    inspect_kv "kind" "special"
  fi

  if inspect_cmd_exists stat; then
    inspect_kv "mode" "$(stat -c '%A %a' "$path" 2>/dev/null || stat -f '%Sp %Lp' "$path" 2>/dev/null || true)"
    inspect_kv "owner" "$(stat -c '%U:%G' "$path" 2>/dev/null || stat -f '%Su:%Sg' "$path" 2>/dev/null || true)"
    inspect_kv "modified" "$(stat -c '%y' "$path" 2>/dev/null || stat -f '%Sm' "$path" 2>/dev/null || true)"
  fi

  if [[ -f "$path" ]]; then
    inspect_kv "size" "$(du -h "$path" 2>/dev/null | awk '{print $1}')"

    if inspect_cmd_exists file; then
      inspect_kv "file" "$(file -b "$path" 2>/dev/null)"
    fi

    if inspect_cmd_exists sha256sum; then
      inspect_kv "sha256" "$(sha256sum "$path" | awk '{print $1}')"
    elif inspect_cmd_exists shasum; then
      inspect_kv "sha256" "$(shasum -a 256 "$path" | awk '{print $1}')"
    fi
  fi
}

inspect_tree() {
  local dir="$1"
  local depth="${2:-3}"

  [[ -d "$dir" ]] || {
    inspect_warn_kv "tree" "directory missing: $dir"
    return 0
  }

  if inspect_cmd_exists tree; then
    tree -a -L "$depth" "$dir"
  else
    find "$dir" -maxdepth "$depth" -print | sed "s#^$dir#.#"
  fi
}

inspect_dir_summary() {
  local dir="$1"

  [[ -d "$dir" ]] || {
    inspect_fail_kv "directory" "missing: $dir"
    return 0
  }

  inspect_kv "path" "$dir"
  inspect_kv "size" "$(du -sh "$dir" 2>/dev/null | awk '{print $1}')"
  inspect_kv "files" "$(find "$dir" -type f 2>/dev/null | wc -l | awk '{print $1}')"
  inspect_kv "dirs" "$(find "$dir" -type d 2>/dev/null | wc -l | awk '{print $1}')"

  local cmake_gut
  cmake_gut="$(find "$dir" \( \
    -name CMakeFiles -o \
    -name CMakeCache.txt -o \
    -name compile_commands.json -o \
    -name Makefile -o \
    -name cmake_install.cmake \
  \) -print -quit 2>/dev/null || true)"

  if [[ -z "$cmake_gut" ]]; then
    inspect_ok_kv "hygiene" "no CMake guts detected"
  else
    inspect_fail_kv "hygiene" "build artifact leaked: $cmake_gut"
  fi
}

inspect_largest_files() {
  local dir="$1"
  local count="${2:-15}"

  [[ -d "$dir" ]] || return 0

  inspect_section "Largest files"
  find "$dir" -type f -printf '%s %p\n' 2>/dev/null \
    | sort -nr \
    | head -n "$count" \
    | while read -r size path; do
        printf '  %8s  %s\n' "$(numfmt --to=iec --suffix=B "$size" 2>/dev/null || printf '%sB' "$size")" "$path"
      done
}

inspect_resource_summary() {
  local resource_dir="$1"

  [[ -d "$resource_dir" ]] || {
    inspect_warn_kv "resources" "missing: $resource_dir"
    return 0
  }

  inspect_section "Resources"
  inspect_dir_summary "$resource_dir"
  inspect_kv "fonts" "$(find "$resource_dir" -type f \( -name '*.ttf' -o -name '*.otf' \) 2>/dev/null | wc -l | awk '{print $1}')"
  inspect_kv "svg icons" "$(find "$resource_dir" -type f -name '*.svg' 2>/dev/null | wc -l | awk '{print $1}')"
  inspect_kv "png images" "$(find "$resource_dir" -type f -name '*.png' 2>/dev/null | wc -l | awk '{print $1}')"
}

inspect_plist() {
  local plist="$1"

  inspect_section "Info.plist"

  [[ -f "$plist" ]] || {
    inspect_warn_kv "Info.plist" "missing: $plist"
    return 0
  }

  inspect_file "$plist"

  if inspect_cmd_exists python3; then
    python3 - "$plist" <<'PY'
import plistlib
import sys

path = sys.argv[1]
keys = [
    "CFBundleIdentifier",
    "CFBundleName",
    "CFBundleDisplayName",
    "CFBundleExecutable",
    "CFBundleVersion",
    "CFBundleShortVersionString",
    "CFBundlePackageType",
    "MinimumOSVersion",
    "LSMinimumSystemVersion",
    "UIDeviceFamily",
    "UILaunchStoryboardName",
    "NSPrincipalClass",
]

try:
    with open(path, "rb") as f:
        data = plistlib.load(f)
except Exception as exc:
    print(f"  ! plist parse            {exc}")
    raise SystemExit(0)

for key in keys:
    if key in data:
        print(f"  {key:<24} {data[key]}")
PY
  else
    inspect_warn_kv "plist parse" "python3 unavailable"
  fi
}

inspect_zip() {
  local zip_path="$1"

  inspect_section "Zip / archive"
  inspect_file "$zip_path"

  [[ -f "$zip_path" ]] || return 0

  if inspect_cmd_exists unzip; then
    inspect_kv "entries" "$(unzip -l "$zip_path" 2>/dev/null | awk '/files$/ {print $2}' | tail -n 1)"
    unzip -l "$zip_path" 2>/dev/null | sed -n '1,12p'
  elif inspect_cmd_exists zipinfo; then
    zipinfo "$zip_path" | sed -n '1,20p'
  else
    inspect_warn_kv "archive listing" "unzip/zipinfo unavailable"
  fi
}

inspect_macho_with_tool() {
  local bin="$1"
  local otool_cmd="$2"
  local nm_cmd="$3"

  if [[ -n "$otool_cmd" && -x "$otool_cmd" ]]; then
    inspect_section "Mach-O linked libraries"
    "$otool_cmd" -L "$bin" 2>/dev/null | sed 's/^/  /' || true

    inspect_section "Mach-O load commands, first 120 lines"
    "$otool_cmd" -l "$bin" 2>/dev/null | sed -n '1,120p' | sed 's/^/  /' || true
  else
    inspect_warn_kv "otool" "not available for $bin"
  fi

  if [[ -n "$nm_cmd" && -x "$nm_cmd" ]]; then
    inspect_section "Mach-O symbols summary"
    inspect_kv "undefined" "$("$nm_cmd" -u "$bin" 2>/dev/null | wc -l | awk '{print $1}')"
    inspect_kv "external" "$("$nm_cmd" -g "$bin" 2>/dev/null | wc -l | awk '{print $1}')"
  fi
}

inspect_linux_binary() {
  local bin="$1"

  inspect_section "Linux binary"
  inspect_file "$bin"

  [[ -f "$bin" ]] || return 0

  inspect_section "Linux dynamic dependencies"
  if inspect_cmd_exists ldd; then
    ldd "$bin" 2>&1 | sed 's/^/  /'
  else
    inspect_warn_kv "ldd" "missing"
  fi

  if inspect_cmd_exists readelf; then
    inspect_section "ELF header"
    readelf -h "$bin" 2>/dev/null | sed 's/^/  /'

    inspect_section "ELF program interpreter and dynamic section"
    readelf -l "$bin" 2>/dev/null | grep -A1 'Requesting program interpreter' | sed 's/^/  /' || true
    readelf -d "$bin" 2>/dev/null | sed 's/^/  /'
  fi

  if inspect_cmd_exists nm; then
    inspect_section "ELF symbol summary"
    inspect_kv "undefined" "$(nm -u "$bin" 2>/dev/null | wc -l | awk '{print $1}')"
    inspect_kv "external" "$(nm -g "$bin" 2>/dev/null | wc -l | awk '{print $1}')"
  fi
}

inspect_windows_binary() {
  local bin="$1"

  inspect_section "Windows executable"
  inspect_file "$bin"

  [[ -f "$bin" ]] || return 0

  local objdump_cmd=""
  if inspect_cmd_exists x86_64-w64-mingw32-objdump; then
    objdump_cmd="$(command -v x86_64-w64-mingw32-objdump)"
  elif inspect_cmd_exists objdump; then
    objdump_cmd="$(command -v objdump)"
  fi

  if [[ -n "$objdump_cmd" ]]; then
    inspect_section "PE headers"
    "$objdump_cmd" -f "$bin" 2>/dev/null | sed 's/^/  /' || true

    inspect_section "PE imported DLLs"
    "$objdump_cmd" -p "$bin" 2>/dev/null \
      | grep -E 'DLL Name:|Subsystem|ImageBase|SectionAlignment|FileAlignment|MajorSubsystemVersion|MinorSubsystemVersion' \
      | sed 's/^/  /' || true
  else
    inspect_warn_kv "objdump" "missing"
  fi
}

inspect_macos_bundle() {
  local app="$1"
  local bin="$app/Contents/MacOS/$APP_NAME"
  local plist="$app/Contents/Info.plist"
  local resources="$app/Contents/Resources"
  local macos_otool="${MACOS_TOOL_BIN:-}/x86_64-apple-darwin-otool"
  local macos_nm="${MACOS_TOOL_BIN:-}/x86_64-apple-darwin-nm"

  inspect_title "macOS bundle"
  inspect_dir_summary "$app"
  inspect_tree "$app" 4
  inspect_plist "$plist"

  inspect_section "macOS app binary"
  inspect_file "$bin"
  [[ -f "$bin" ]] && inspect_macho_with_tool "$bin" "$macos_otool" "$macos_nm"

  inspect_resource_summary "$resources"

  inspect_section "macOS bundle extras"
  inspect_path "icns" "$resources/$APP_NAME.icns"
  inspect_path "MacOS dir" "$app/Contents/MacOS"
  inspect_path "Resources dir" "$resources"
}

inspect_ios_bundle() {
  local root="$1"
  local app="$root/$APP_NAME.app"
  local bin="$app/$APP_NAME"
  local plist="$app/Info.plist"
  local ipa="$root/$APP_NAME.ipa"
  local ios_otool="${IOS_TOOL_BIN:-}/aarch64-apple-darwin-otool"
  local ios_nm="${IOS_TOOL_BIN:-}/aarch64-apple-darwin-nm"

  inspect_title "iOS bundle"
  inspect_dir_summary "$root"
  inspect_tree "$root" 3

  inspect_section "iOS .app"
  inspect_dir_summary "$app"
  inspect_plist "$plist"

  inspect_section "iOS app binary"
  inspect_file "$bin"
  [[ -f "$bin" ]] && inspect_macho_with_tool "$bin" "$ios_otool" "$ios_nm"

  inspect_resource_summary "$app"

  inspect_section "iOS signing and icons"
  inspect_path "CodeResources" "$app/_CodeSignature/CodeResources"
  inspect_kv "app icon pngs" "$(find "$app" -maxdepth 1 -type f -name 'Icon*.png' 2>/dev/null | wc -l | awk '{print $1}')"

  inspect_zip "$ipa"
}

inspect_linux_bundle() {
  local dir="$1"
  local bin="$dir/$APP_NAME"

  inspect_title "Linux package"
  inspect_dir_summary "$dir"
  inspect_tree "$dir" 3
  inspect_linux_binary "$bin"
  inspect_resource_summary "$dir/resources"
}

inspect_windows_bundle() {
  local dir="$1"
  local bin="$dir/$APP_NAME.exe"

  inspect_title "Windows package"
  inspect_dir_summary "$dir"
  inspect_tree "$dir" 3
  inspect_windows_binary "$bin"
  inspect_resource_summary "$dir/resources"
}

inspect_full_zip() {
  local zip_path="$PROJECT_ROOT/full_package/$APP_NAME-all.zip"

  inspect_title "Full release zip"
  inspect_zip "$zip_path"
}

inspect_all_summary() {
  inspect_title "Synthem release inspection"
  inspect_kv "project" "$PROJECT_ROOT"
  inspect_kv "app" "${APP_NAME:-Synthem}"
  inspect_kv "full package" "${BUILD_DIR_FULL_PACKAGE:-$PROJECT_ROOT/full_package}"

  if inspect_cmd_exists git && git -C "$PROJECT_ROOT" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    inspect_kv "git branch" "$(git -C "$PROJECT_ROOT" branch --show-current 2>/dev/null)"
    inspect_kv "git commit" "$(git -C "$PROJECT_ROOT" rev-parse --short HEAD 2>/dev/null)"
    inspect_kv "git dirty" "$(if [[ -n "$(git -C "$PROJECT_ROOT" status --porcelain 2>/dev/null)" ]]; then printf yes; else printf no; fi)"
  fi
}

cmd_inspect() {
  local target="${1:-all}"

  APP_NAME="${APP_NAME:-Synthem}"
  BUILD_DIR_FULL_PACKAGE="${BUILD_DIR_FULL_PACKAGE:-$PROJECT_ROOT/full_package}"
  BUILD_LINUX_DIR="${BUILD_LINUX_DIR:-build_linux}"
  BUILD_WINDOWS_DIR="${BUILD_WINDOWS_DIR:-build_windows}"
  BUILD_MACOS_DIR="${BUILD_MACOS_DIR:-build_macos}"
  BUILD_IOS_DIR="${BUILD_IOS_DIR:-build_ios}"

  IOS_TOOLCHAIN_ROOT="${SYNTHEM_IOS_TOOLCHAIN_ROOT:-/home/code/procursus/cctools-port/usage_examples/ios_toolchain}"
  MACOS_TOOLCHAIN_ROOT="${SYNTHEM_MACOS_TOOLCHAIN_ROOT:-/home/code/procursus/cctools-port/usage_examples/macos_toolchain}"
  IOS_TOOL_BIN="${IOS_TOOLCHAIN_ROOT}/target/bin"
  MACOS_TOOL_BIN="${MACOS_TOOLCHAIN_ROOT}/target/bin"

  case "$target" in
    all)
      inspect_all_summary
      inspect_linux_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_LINUX_DIR"
      inspect_windows_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_WINDOWS_DIR"
      inspect_macos_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_MACOS_DIR/$APP_NAME.app"
      inspect_ios_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_IOS_DIR"
      inspect_full_zip
      ;;
    linux)
      inspect_linux_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_LINUX_DIR"
      ;;
    windows)
      inspect_windows_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_WINDOWS_DIR"
      ;;
    macos)
      inspect_macos_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_MACOS_DIR/$APP_NAME.app"
      ;;
    ios)
      inspect_ios_bundle "$BUILD_DIR_FULL_PACKAGE/$BUILD_IOS_DIR"
      ;;
    zip)
      inspect_full_zip
      ;;
    *)
      die "invalid inspect target: $target"
      ;;
  esac
}
