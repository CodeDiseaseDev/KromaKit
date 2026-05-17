#!/usr/bin/env bash
set -euo pipefail



# Current compatibility layout (legacy paths in active use today).
# These are relative to the project root and must remain stable in this phase.
BUILD_IOS_OBJ_DIR="build"
BUILD_IOS_PACKAGE_DIR="build_ios"
BUILD_MACOS_DIR="build_macos"
BUILD_MACOS_PACKAGE_DIR="build_macos_package"
BUILD_LINUX_DIR="build_linux"
BUILD_WINDOWS_DIR="build_windows"
BUILD_ICONS_DIR="build_icons"
BUILD_FULL_PACKAGE_DIR="full_package"
BUILD_TESTS_DIR="build_tests"

# Future canonical layout (defined now, not used yet).
# Scripts should migrate to these in later phases.
BUILD_ROOT="build"

IOS_BUILD_ROOT="$BUILD_ROOT/ios"
IOS_OBJ_DIR="$IOS_BUILD_ROOT/obj"
IOS_BIN_DIR="$IOS_BUILD_ROOT/bin"
IOS_PACKAGE_DIR="$IOS_BUILD_ROOT/package"
IOS_ARTIFACTS_DIR="$IOS_BUILD_ROOT/artifacts"
IOS_LOGS_DIR="$IOS_BUILD_ROOT/logs"

MACOS_BUILD_ROOT="$BUILD_ROOT/macos"
MACOS_OBJ_DIR="$MACOS_BUILD_ROOT/obj"
MACOS_BIN_DIR="$MACOS_BUILD_ROOT/bin"
MACOS_PACKAGE_DIR="$MACOS_BUILD_ROOT/package"
MACOS_ARTIFACTS_DIR="$MACOS_BUILD_ROOT/artifacts"
MACOS_LOGS_DIR="$MACOS_BUILD_ROOT/logs"

LINUX_BUILD_ROOT="$BUILD_ROOT/linux"
LINUX_OBJ_DIR="$LINUX_BUILD_ROOT/obj"
LINUX_BIN_DIR="$LINUX_BUILD_ROOT/bin"
LINUX_PACKAGE_DIR="$LINUX_BUILD_ROOT/package"
LINUX_ARTIFACTS_DIR="$LINUX_BUILD_ROOT/artifacts"
LINUX_LOGS_DIR="$LINUX_BUILD_ROOT/logs"

WINDOWS_BUILD_ROOT="$BUILD_ROOT/windows"
WINDOWS_OBJ_DIR="$WINDOWS_BUILD_ROOT/obj"
WINDOWS_BIN_DIR="$WINDOWS_BUILD_ROOT/bin"
WINDOWS_PACKAGE_DIR="$WINDOWS_BUILD_ROOT/package"
WINDOWS_ARTIFACTS_DIR="$WINDOWS_BUILD_ROOT/artifacts"
WINDOWS_LOGS_DIR="$WINDOWS_BUILD_ROOT/logs"

SHARED_BUILD_ROOT="$BUILD_ROOT/shared"
SHARED_ICONS_DIR="$SHARED_BUILD_ROOT/icons"
