#!/usr/bin/env bash
set -euo pipefail

SYNTHEM_IOS_TOOLCHAIN_ROOT="${SYNTHEM_IOS_TOOLCHAIN_ROOT:-/home/code/procursus/cctools-port/usage_examples/ios_toolchain}"
SYNTHEM_MACOS_TOOLCHAIN_ROOT="${SYNTHEM_MACOS_TOOLCHAIN_ROOT:-/home/code/procursus/cctools-port/usage_examples/macos_toolchain}"

resolve_tool_in_bin() {
  local tool_bin="$1"
  local prefix="$2"
  local tool="$3"

  if [[ -x "$tool_bin/$tool" ]]; then
    printf '%s\n' "$tool_bin/$tool"
    return 0
  fi

  if [[ -x "$tool_bin/$prefix-$tool" ]]; then
    printf '%s\n' "$tool_bin/$prefix-$tool"
    return 0
  fi

  return 1
}

verify_apple_toolchain() {
  local platform="$1"
  local toolchain_root="$2"
  local tool_prefix="$3"
  local tool_bin="$toolchain_root/target/bin"
  local required_tools="clang clang++ ld ar ranlib nm"
  local tool resolved

  [[ -d "$toolchain_root" ]] || die "$platform toolchain root does not exist: $toolchain_root"
  [[ -d "$tool_bin" ]] || die "$platform toolchain target/bin does not exist: $tool_bin"

  for tool in $required_tools; do
    resolved="$(resolve_tool_in_bin "$tool_bin" "$tool_prefix" "$tool")" || \
      die "$platform toolchain missing $tool in $tool_bin"
    log "$platform tool: $tool -> $resolved"
  done
}

cmake_jobs() {
  printf '%s' "${SYNTHEM_JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"
}

linux_env_args() {
  printf '%s\n' \
    "HOME=$HOME" \
    "USER=${USER:-$(whoami)}" \
    "SHELL=${SHELL:-/bin/bash}" \
    "TERM=${TERM:-xterm-256color}" \
    "PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin"
}

procursus_env_args() {
  printf '%s\n' \
    "PATH=/opt/procursus/bin:/opt/procursus/sbin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin" \
    "PROCURRENT=1"
}

native_env_args() {
  printf '%s\n' \
    "PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH"
}

cmake_configure_linux() {
  local build_dir="$1"
  shift

  rm -rf "$build_dir"

  run env -i \
    HOME="$HOME" \
    USER="${USER:-$(whoami)}" \
    SHELL="${SHELL:-/bin/bash}" \
    TERM="${TERM:-xterm-256color}" \
    PATH="/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin" \
    cmake -B "$build_dir" -S "$PROJECT_ROOT" \
      -DCMAKE_BUILD_TYPE="${SYNTHEM_MODE:-Debug}" \
      "$@"
}

build_windows() {
  local build_dir="$PROJECT_ROOT/build_windows"

  cmake_configure_windows "$build_dir"
  cmake_build_dir "$build_dir"
}

cmake_configure_native() {
  local build_dir="$1"
  shift

  rm -rf "$build_dir"

  run env \
    PATH="/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:$PATH" \
    cmake -B "$build_dir" -S "$PROJECT_ROOT" \
      -DCMAKE_BUILD_TYPE="${SYNTHEM_MODE:-Debug}" \
      "$@"
}

cmake_configure_ios() {
  local build_dir="$1"
  local tool_bin="$SYNTHEM_IOS_TOOLCHAIN_ROOT/target/bin"
  shift

  verify_apple_toolchain "iOS" "$SYNTHEM_IOS_TOOLCHAIN_ROOT" "aarch64-apple-darwin"

  rm -rf "$build_dir"

  run env -i \
    HOME="$HOME" \
    USER="${USER:-$(whoami)}" \
    SHELL="${SHELL:-/bin/bash}" \
    TERM="${TERM:-xterm-256color}" \
    PATH="$tool_bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin" \
    SYNTHEM_IOS_TOOLCHAIN_ROOT="$SYNTHEM_IOS_TOOLCHAIN_ROOT" \
    cmake -B "$build_dir" -S "$PROJECT_ROOT" \
      -DCMAKE_BUILD_TYPE="${SYNTHEM_MODE:-Debug}" \
      "$@"
}

cmake_configure_macos() {
  local build_dir="$1"
  local tool_bin="$SYNTHEM_MACOS_TOOLCHAIN_ROOT/target/bin"
  shift

  verify_apple_toolchain "macOS" "$SYNTHEM_MACOS_TOOLCHAIN_ROOT" "x86_64-apple-darwin"

  rm -rf "$build_dir"

  run env -i \
    HOME="$HOME" \
    USER="${USER:-$(whoami)}" \
    SHELL="${SHELL:-/bin/bash}" \
    TERM="${TERM:-xterm-256color}" \
    PATH="$tool_bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin" \
    SYNTHEM_MACOS_TOOLCHAIN_ROOT="$SYNTHEM_MACOS_TOOLCHAIN_ROOT" \
    cmake -B "$build_dir" -S "$PROJECT_ROOT" \
      -DCMAKE_BUILD_TYPE="${SYNTHEM_MODE:-Debug}" \
      "$@"
}

cmake_build_dir() {
  local build_dir="$1"

  run cmake \
    --build "$build_dir" \
    --config "${SYNTHEM_MODE:-Debug}" \
    -j "$(cmake_jobs)"
}

build_linux() {
  local build_dir="$PROJECT_ROOT/build_linux"

  cmake_configure_linux "$build_dir" \
    -DCMAKE_C_COMPILER=/usr/bin/gcc \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++

  cmake_build_dir "$build_dir"
}

cmake_configure_windows() {
  local build_dir="$1"
  shift

  rm -rf "$build_dir"

  run env -i \
    HOME="$HOME" \
    USER="${USER:-$(whoami)}" \
    SHELL="${SHELL:-/bin/bash}" \
    TERM="${TERM:-xterm-256color}" \
    PATH="/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin" \
    cmake -B "$build_dir" -S "$PROJECT_ROOT" \
      -DCMAKE_BUILD_TYPE="${SYNTHEM_MODE:-Debug}" \
      -DCMAKE_SYSTEM_NAME=Windows \
      -DCMAKE_C_COMPILER=/usr/bin/x86_64-w64-mingw32-gcc \
      -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-w64-mingw32-g++ \
      "$@"
}

build_macos() {
  local build_dir="$PROJECT_ROOT/build_macos"

  cmake_configure_macos "$build_dir" \
    -DCMAKE_TOOLCHAIN_FILE="$PROJECT_ROOT/cmake/toolchains/macos-procursus.cmake"

  cmake_build_dir "$build_dir"
}

build_ios() {
  local build_dir="$PROJECT_ROOT/build_ios"

  cmake_configure_ios "$build_dir" \
    -DCMAKE_TOOLCHAIN_FILE="$PROJECT_ROOT/cmake/toolchains/ios-procursus.cmake"

  cmake_build_dir "$build_dir"
}

build_platform() {
  local platform="$1"

  case "$platform" in
    ios)
      build_ios
      ;;
    macos)
      build_macos
      ;;
    linux)
      build_linux
      ;;
    windows)
      build_windows
      ;;
    *)
      die "unsupported build platform: $platform"
      ;;
  esac
}

cmd_build() {
  local target="${1:-all}"
  target="$(normalize_platform "$target")" || die "invalid build target: $target"

  log "target: $target"
  log "mode: $SYNTHEM_MODE"
  log "jobs: $SYNTHEM_JOBS"

  local build_targets
  build_targets="$(build_targets_for "$target")" || die "invalid build target: $target"

  local total=0
  local platform
  for platform in $build_targets; do
    total=$((total + 1))
  done

  local i=0
  for platform in $build_targets; do
    i=$((i + 1))
    step "$i" "$total" "build $platform"
    build_platform "$platform"
  done
}
