DOCTOR_OK=0
DOCTOR_WARN=0
DOCTOR_FAIL=0

IOS_TOOLCHAIN_ROOT="${SYNTHEM_IOS_TOOLCHAIN_ROOT:-/home/code/procursus/cctools-port/usage_examples/ios_toolchain}"
MACOS_TOOLCHAIN_ROOT="${SYNTHEM_MACOS_TOOLCHAIN_ROOT:-/home/code/procursus/cctools-port/usage_examples/macos_toolchain}"

IOS_TOOL_BIN="$IOS_TOOLCHAIN_ROOT/target/bin"
MACOS_TOOL_BIN="$MACOS_TOOLCHAIN_ROOT/target/bin"

IOS_SYSROOT="${SYNTHEM_IOS_SYSROOT:-${IOS_SYSROOT:-/home/code/SDKs/iPhoneOS13.2.sdk}}"
MACOS_SYSROOT="${SYNTHEM_MACOS_SYSROOT:-${MACOS_SYSROOT:-/home/code/SDKs/MacOSX14.5.sdk}}"

IOS_LDID_BIN="${IOS_LDID_BIN:-/home/code/ldid/ldid}"
WINDOWS_RCEDIT_BIN="${WINDOWS_RCEDIT_BIN:-$PROJECT_ROOT/scripts/tools/rcedit-x64.exe}"

DOCTOR_DEVICE_HOST="${SYNTHEM_DEVICE_HOST:-mobile@192.168.0.133}"

doctor_reset() {
  DOCTOR_OK=0
  DOCTOR_WARN=0
  DOCTOR_FAIL=0
}

doctor_mark_ok() {
  DOCTOR_OK=$((DOCTOR_OK + 1))
}

doctor_mark_warn() {
  DOCTOR_WARN=$((DOCTOR_WARN + 1))
}

doctor_mark_fail() {
  DOCTOR_FAIL=$((DOCTOR_FAIL + 1))
}

doctor_row() {
  local state="$1"
  local label="$2"
  local detail="${3:-}"

  case "$state" in
    ok)
      doctor_mark_ok
      printf '  %s✓%s %-34s %s%s%s\n' "$C_GREEN" "$C_RESET" "$label" "$C_DIM" "$detail" "$C_RESET"
      ;;
    warn)
      doctor_mark_warn
      printf '  %s!%s %-34s %s%s%s\n' "$C_YELLOW" "$C_RESET" "$label" "$C_DIM" "$detail" "$C_RESET"
      ;;
    fail)
      doctor_mark_fail
      printf '  %s✗%s %-34s %s%s%s\n' "$C_RED" "$C_RESET" "$label" "$C_DIM" "$detail" "$C_RESET"
      ;;
  esac
}

doctor_section() {
  printf '\n%s%s%s\n' "$C_BOLD$C_CYAN" "$1" "$C_RESET"
}

doctor_cmd() {
  local label="$1"
  local cmd="$2"
  local level="${3:-required}"
  local path=""

  if path="$(command -v "$cmd" 2>/dev/null)"; then
    doctor_row ok "$label" "$path"
  else
    if [[ "$level" == "optional" ]]; then
      doctor_row warn "$label" "missing optional command: $cmd"
    else
      doctor_row fail "$label" "missing command: $cmd"
    fi
  fi
}

doctor_file() {
  local label="$1"
  local path="$2"
  local level="${3:-required}"

  if [[ -f "$path" ]]; then
    doctor_row ok "$label" "$path"
  else
    if [[ "$level" == "optional" ]]; then
      doctor_row warn "$label" "missing optional file: $path"
    else
      doctor_row fail "$label" "missing file: $path"
    fi
  fi
}

doctor_exe() {
  local label="$1"
  local path="$2"
  local level="${3:-required}"

  if [[ -x "$path" ]]; then
    doctor_row ok "$label" "$path"
  elif [[ -f "$path" ]]; then
    doctor_row warn "$label" "exists but is not executable: $path"
  else
    if [[ "$level" == "optional" ]]; then
      doctor_row warn "$label" "missing optional executable: $path"
    else
      doctor_row fail "$label" "missing executable: $path"
    fi
  fi
}

doctor_dir() {
  local label="$1"
  local path="$2"
  local level="${3:-required}"

  if [[ -d "$path" ]]; then
    doctor_row ok "$label" "$path"
  else
    if [[ "$level" == "optional" ]]; then
      doctor_row warn "$label" "missing optional directory: $path"
    else
      doctor_row fail "$label" "missing directory: $path"
    fi
  fi
}

doctor_pkg() {
  local label="$1"
  local pkg="$2"
  local level="${3:-required}"
  local version=""

  if ! command -v pkg-config >/dev/null 2>&1; then
    doctor_row warn "$label" "pkg-config unavailable"
    return 0
  fi

  if pkg-config --exists "$pkg" 2>/dev/null; then
    version="$(pkg-config --modversion "$pkg" 2>/dev/null || true)"
    doctor_row ok "$label" "${pkg}${version:+ $version}"
  else
    if [[ "$level" == "optional" ]]; then
      doctor_row warn "$label" "pkg-config missing optional package: $pkg"
    else
      doctor_row fail "$label" "pkg-config missing package: $pkg"
    fi
  fi
}

doctor_toolchain_tool() {
  local label="$1"
  local bin_dir="$2"
  local tool="$3"
  local level="${4:-required}"

  doctor_exe "$label" "$bin_dir/$tool" "$level"
}

doctor_framework() {
  local label="$1"
  local sdk="$2"
  local framework="$3"
  local level="${4:-required}"
  local path="$sdk/System/Library/Frameworks/$framework.framework"

  doctor_dir "$label" "$path" "$level"
}

doctor_build_artifact_absent() {
  local label="$1"
  local path="$2"

  if [[ -e "$path" ]]; then
    doctor_row warn "$label" "present: $path"
  else
    doctor_row ok "$label" "not present"
  fi
}

doctor_output_file() {
  local label="$1"
  local path="$2"
  local level="${3:-optional}"
  local size=""

  if [[ -f "$path" ]]; then
    size="$(du -h "$path" 2>/dev/null | awk '{print $1}')"
    doctor_row ok "$label" "${path}${size:+ ($size)}"
  else
    if [[ "$level" == "required" ]]; then
      doctor_row fail "$label" "missing file: $path"
    else
      doctor_row warn "$label" "not built yet: $path"
    fi
  fi
}

doctor_no_packaging_guts() {
  local package_dir="$PROJECT_ROOT/full_package"
  local found=""

  if [[ ! -d "$package_dir" ]]; then
    doctor_row warn "package hygiene" "full_package does not exist yet"
    return 0
  fi

  found="$(find "$package_dir" \( \
    -name CMakeFiles -o \
    -name CMakeCache.txt -o \
    -name compile_commands.json -o \
    -name Makefile -o \
    -name cmake_install.cmake \
  \) -print -quit 2>/dev/null || true)"

  if [[ -z "$found" ]]; then
    doctor_row ok "package hygiene" "no CMake guts found in full_package"
  else
    doctor_row fail "package hygiene" "build artifact leaked: $found"
  fi
}

doctor_gitignore_tracking() {
  local tracked=""

  if ! command -v git >/dev/null 2>&1 || ! git -C "$PROJECT_ROOT" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    doctor_row warn "git tracked build dirs" "not a git work tree"
    return 0
  fi

  tracked="$(git -C "$PROJECT_ROOT" ls-files \
    build_icons build_ios build_linux build_macos build_macos_package build_tests build_windows full_package 2>/dev/null || true)"

  if [[ -z "$tracked" ]]; then
    doctor_row ok "git tracked build dirs" "none"
  else
    doctor_row fail "git tracked build dirs" "$(tr '\n' ' ' <<< "$tracked")"
  fi
}

doctor_device_ping() {
  local label="$1"
  local target="$2"
  local host="$target"

  # Accept either "mobile@192.168.0.133" or just "192.168.0.133".
  if [[ "$host" == *@* ]]; then
    host="${host#*@}"
  fi

  if [[ -z "$host" ]]; then
    doctor_row warn "$label" "no host configured"
    return 0
  fi

  if ! command -v ping >/dev/null 2>&1; then
    doctor_row warn "$label" "ping command missing"
    return 0
  fi

  if ping -c 1 -W 1 "$host" >/dev/null 2>&1; then
    doctor_row ok "$label" "$host reachable"
  else
    doctor_row warn "$label" "$host not reachable"
  fi
}

doctor_device_ssh() {
  local label="$1"
  local target="$2"

  if ! command -v ssh >/dev/null 2>&1; then
    doctor_row warn "$label" "ssh command missing"
    return 0
  fi

  if ssh -o BatchMode=yes -o ConnectTimeout=2 "$target" true >/dev/null 2>&1; then
    doctor_row ok "$label" "$target ssh reachable"
  else
    doctor_row warn "$label" "$target ssh not reachable"
  fi
}

doctor() {
  doctor_reset

  printf '%s%s%s\n' "$C_BOLD$C_PURPLE" "╭──────────────────────────────────────╮" "$C_RESET"
  printf '%s%s%s\n' "$C_BOLD$C_PURPLE" "│  Synthem doctor · toolchain checkup  │" "$C_RESET"
  printf '%s%s%s\n' "$C_BOLD$C_PURPLE" "╰──────────────────────────────────────╯" "$C_RESET"

  doctor_section "Project"
  doctor_file "synthem.sh" "$PROJECT_ROOT/synthem.sh"
  doctor_dir "project root" "$PROJECT_ROOT"
  doctor_gitignore_tracking

  doctor_section "Core build tools"
  doctor_cmd "bash" bash
  doctor_cmd "make" make
  doctor_cmd "cmake" cmake
  doctor_cmd "clang" clang
  doctor_cmd "clang++" clang++
  doctor_cmd "python3" python3
  doctor_cmd "pkg-config" pkg-config
  doctor_cmd "zip" zip
  doctor_cmd "rsync" rsync
  doctor_cmd "tree" tree optional
  doctor_cmd "bear" bear optional
  doctor_cmd "lldb" lldb optional

  doctor_section "Linux build deps"
  doctor_pkg "glfw3" glfw3 optional
  doctor_pkg "alsa" alsa optional
  doctor_pkg "pipewire" libpipewire-0.3 optional
  doctor_cmd "OpenGL headers/libs hint" glxinfo optional

  doctor_section "Windows cross build deps"
  doctor_cmd "mingw g++" x86_64-w64-mingw32-g++
  doctor_cmd "mingw gcc" x86_64-w64-mingw32-gcc optional
  doctor_cmd "mingw strip" x86_64-w64-mingw32-strip optional
  doctor_cmd "wine" wine
  doctor_file "rcedit" "$WINDOWS_RCEDIT_BIN" optional

  doctor_section "Icon and packaging tools"
  doctor_cmd "rsvg-convert" rsvg-convert
  doctor_cmd "ImageMagick magick" magick
  doctor_cmd "png2icns" png2icns optional
  doctor_cmd "dd" dd
  doctor_cmd "mount" mount optional
  doctor_cmd "umount" umount optional
  doctor_cmd "sudo" sudo optional
  doctor_cmd "sha256sum" sha256sum

  doctor_section "iOS cross toolchain"
  doctor_dir "iOS toolchain root" "$IOS_TOOLCHAIN_ROOT"
  doctor_dir "iOS target/bin" "$IOS_TOOL_BIN"
  doctor_toolchain_tool "iOS clang" "$IOS_TOOL_BIN" "aarch64-apple-darwin-clang"
  doctor_toolchain_tool "iOS clang++" "$IOS_TOOL_BIN" "aarch64-apple-darwin-clang++"
  doctor_toolchain_tool "iOS ld" "$IOS_TOOL_BIN" "ld"
  doctor_toolchain_tool "iOS ar" "$IOS_TOOL_BIN" "aarch64-apple-darwin-ar"
  doctor_toolchain_tool "iOS ranlib" "$IOS_TOOL_BIN" "aarch64-apple-darwin-ranlib"
  doctor_toolchain_tool "iOS nm" "$IOS_TOOL_BIN" "aarch64-apple-darwin-nm"
  doctor_toolchain_tool "iOS strip" "$IOS_TOOL_BIN" "aarch64-apple-darwin-strip" optional
  doctor_dir "iPhoneOS SDK" "$IOS_SYSROOT"
  doctor_exe "ldid" "$IOS_LDID_BIN"

  doctor_framework "UIKit" "$IOS_SYSROOT" UIKit
  doctor_framework "Foundation iOS" "$IOS_SYSROOT" Foundation
  doctor_framework "AVFoundation iOS" "$IOS_SYSROOT" AVFoundation
  doctor_framework "AudioToolbox iOS" "$IOS_SYSROOT" AudioToolbox
  doctor_framework "Metal iOS" "$IOS_SYSROOT" Metal
  doctor_framework "MetalKit iOS" "$IOS_SYSROOT" MetalKit
  doctor_framework "MPS iOS" "$IOS_SYSROOT" MetalPerformanceShaders
  doctor_framework "QuartzCore iOS" "$IOS_SYSROOT" QuartzCore
  doctor_framework "CoreGraphics iOS" "$IOS_SYSROOT" CoreGraphics

  doctor_section "macOS cross toolchain"
  doctor_dir "macOS toolchain root" "$MACOS_TOOLCHAIN_ROOT"
  doctor_dir "macOS target/bin" "$MACOS_TOOL_BIN"
  doctor_toolchain_tool "macOS clang" "$MACOS_TOOL_BIN" "x86_64-apple-darwin-clang"
  doctor_toolchain_tool "macOS clang++" "$MACOS_TOOL_BIN" "x86_64-apple-darwin-clang++"
  doctor_toolchain_tool "macOS ld" "$MACOS_TOOL_BIN" "ld"
  doctor_toolchain_tool "macOS ar" "$MACOS_TOOL_BIN" "x86_64-apple-darwin-ar"
  doctor_toolchain_tool "macOS ranlib" "$MACOS_TOOL_BIN" "x86_64-apple-darwin-ranlib"
  doctor_toolchain_tool "macOS nm" "$MACOS_TOOL_BIN" "x86_64-apple-darwin-nm"
  doctor_toolchain_tool "macOS strip" "$MACOS_TOOL_BIN" "x86_64-apple-darwin-strip" optional
  doctor_dir "MacOSX SDK" "$MACOS_SYSROOT"

  doctor_framework "AppKit" "$MACOS_SYSROOT" AppKit
  doctor_framework "Foundation macOS" "$MACOS_SYSROOT" Foundation
  doctor_framework "AVFoundation macOS" "$MACOS_SYSROOT" AVFoundation
  doctor_framework "AudioToolbox macOS" "$MACOS_SYSROOT" AudioToolbox
  doctor_framework "Metal macOS" "$MACOS_SYSROOT" Metal
  doctor_framework "MetalKit macOS" "$MACOS_SYSROOT" MetalKit
  doctor_framework "MPS macOS" "$MACOS_SYSROOT" MetalPerformanceShaders
  doctor_framework "QuartzCore macOS" "$MACOS_SYSROOT" QuartzCore
  doctor_framework "CoreGraphics macOS" "$MACOS_SYSROOT" CoreGraphics

  doctor_section "Vendored/local source deps"
  doctor_dir "third_party/glad" "$PROJECT_ROOT/third_party/glad"
  doctor_dir "external/KromaKit" "$PROJECT_ROOT/external/KromaKit"
  doctor_dir "external/KromaKit nanovg" "$PROJECT_ROOT/external/KromaKit/nanovg"
  doctor_file "KromaKit Metal NanoVG" "$PROJECT_ROOT/external/KromaKit/nanovg/nanovg_mtl.mm"
  doctor_dir "external/nanosvg" "$PROJECT_ROOT/external/nanosvg" optional
  doctor_dir "resources" "$PROJECT_ROOT/resources"
  doctor_dir "Font Awesome resources" "$PROJECT_ROOT/resources/icons/fontawesome" optional

  doctor_section "Current outputs"
  doctor_output_file "Linux binary" "$PROJECT_ROOT/build_linux/Synthem"
  doctor_output_file "Windows exe" "$PROJECT_ROOT/build_windows/Synthem.exe"
  doctor_output_file "macOS app binary" "$PROJECT_ROOT/build_macos_package/Synthem.app/Contents/MacOS/Synthem"
  doctor_output_file "iOS raw binary" "$PROJECT_ROOT/build_ios/Synthem"
  doctor_output_file "iOS IPA" "$PROJECT_ROOT/build_ios_package/Synthem.ipa"
  doctor_output_file "full zip" "$PROJECT_ROOT/Synthem-all.zip"
  doctor_no_packaging_guts

  doctor_section "Deploy/debug extras"
  doctor_cmd "ssh" ssh optional
  doctor_cmd "scp" scp optional
  doctor_cmd "ssh-keygen" ssh-keygen optional
  doctor_device_ssh "iPhone ssh" "$DOCTOR_DEVICE_HOST"

  printf '\n%s%s%s\n' "$C_BOLD$C_PURPLE" "Doctor summary" "$C_RESET"
  printf '  %s✓ %d ok%s   %s! %d warn%s   %s✗ %d fail%s\n' \
    "$C_GREEN" "$DOCTOR_OK" "$C_RESET" \
    "$C_YELLOW" "$DOCTOR_WARN" "$C_RESET" \
    "$C_RED" "$DOCTOR_FAIL" "$C_RESET"

  if ((DOCTOR_FAIL > 0)); then
    LAST_STATUS=1
    LAST_DURATION="0s"
    return 1
  fi

  LAST_STATUS=0
  LAST_DURATION="0s"
  return 0
}

cmd_doctor() {
  doctor
}