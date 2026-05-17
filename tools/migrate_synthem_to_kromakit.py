#!/usr/bin/env python3
from __future__ import annotations

import argparse
import os
import re
import shutil
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


TEXT_EXTENSIONS = {
    ".c", ".cc", ".cpp", ".cxx",
    ".h", ".hh", ".hpp", ".hxx",
    ".m", ".mm",
    ".cmake", ".txt", ".md",
    ".mk", ".sh", ".py",
    ".json", ".plist", ".xml",
}

EXCLUDED_DIR_NAMES = {
    ".git",
    ".idea",
    ".vscode",

    # Build/output dirs
    "build",
    "build_linux",
    "build_macos",
    "build_windows",
    "build_package",
    "build_macos_package",
    "build_icons",
    "build_tests",
    "cmake-build-debug",
    "cmake-build-release",
    "out",

    # Old/duplicated framework dirs
    "directuiold",

    # Usually third-party should not be rewritten
    "third_party",
}

EXCLUDED_FILE_NAMES = {
    "compile_commands.json",
}

ROOT_CPP_ALLOWLIST = {
    "main_linux.cpp",
}

ROOT_CPP_DENYLIST = {
    "main.cpp",
    "main.mm",
    "main_macos.mm",
    "main_windows.cpp",
}


@dataclass
class Change:
    path: Path
    description: str


@dataclass
class RewriteResult:
    files_seen: int = 0
    files_changed: int = 0
    replacements: int = 0


def log(message: str) -> None:
    print(message)


def die(message: str) -> None:
    print(f"error: {message}", file=sys.stderr)
    raise SystemExit(1)


def rel(path: Path, root: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return path.as_posix()


def is_under(path: Path, root: Path, child_name: str) -> bool:
    target = root / child_name
    try:
        path.relative_to(target)
        return True
    except ValueError:
        return False


def should_skip_dir(path: Path) -> bool:
    return path.name in EXCLUDED_DIR_NAMES or path.name.startswith("cmake-build-")


def should_scan_text_file(path: Path) -> bool:
    if path.name in EXCLUDED_FILE_NAMES:
        return False

    if path.suffix not in TEXT_EXTENSIONS:
        return False

    return True


def iter_project_files(root: Path) -> Iterable[Path]:
    for dirpath, dirnames, filenames in os.walk(root):
        current = Path(dirpath)

        dirnames[:] = [
            d for d in dirnames
            if not should_skip_dir(current / d)
        ]

        for filename in filenames:
            path = current / filename
            if should_scan_text_file(path):
                yield path


def rewrite_directui_paths(text: str) -> tuple[str, int]:
    """
    Rewrite path references only.

    This intentionally does NOT rename:
      - DUIWindow
      - DUIRect
      - DUIStyleRegistry
      - namespaces
      - class names
      - comments that merely say DirectUI without a path

    It rewrites:
      #include <kromakit/Button.h>
      #include <kromakit/Button.h>
      "kromakit/Button.h"
      kromakit/Button.h
    into:
      #include <kromakit/Button.h>
      "kromakit/Button.h"
      kromakit/Button.h
    """

    replacements = 0

    patterns = [
        # #include <kromakit/Whatever.h> -> #include <kromakit/Whatever.h>
        (
            re.compile(r'(#\s*include\s*)"kromakit/([^"]+)"'),
            lambda m: f'{m.group(1)}<kromakit/{m.group(2)}>',
        ),

        # #include <kromakit/Whatever.h> -> #include <kromakit/Whatever.h>
        (
            re.compile(r'(#\s*include\s*)<kromakit/([^>]+)>'),
            lambda m: f'{m.group(1)}<kromakit/{m.group(2)}>',
        ),

        # Quoted path references in strings/comments
        (
            re.compile(r'"kromakit/([^"]+)"'),
            lambda m: f'"kromakit/{m.group(1)}"',
        ),

        # Angle path references outside include-ish contexts
        (
            re.compile(r'<kromakit/([^>]+)>'),
            lambda m: f'<kromakit/{m.group(1)}>',
        ),

        # Bare path references
        (
            re.compile(r'(?<![A-Za-z0-9_./-])kromakit/'),
            lambda m: 'kromakit/',
        ),
    ]

    new_text = text

    for pattern, repl in patterns:
        new_text, count = pattern.subn(repl, new_text)
        replacements += count

    return new_text, replacements


def rewrite_files(root: Path, apply: bool) -> RewriteResult:
    result = RewriteResult()

    for path in iter_project_files(root):
        result.files_seen += 1

        try:
            original = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue

        rewritten, replacements = rewrite_directui_paths(original)

        if replacements == 0:
            continue

        result.files_changed += 1
        result.replacements += replacements

        log(f"{'rewrite' if apply else 'would rewrite'} {rel(path, root)} ({replacements} replacements)")

        if apply:
            path.write_text(rewritten, encoding="utf-8")

    return result


def rename_directui(root: Path, apply: bool) -> list[Change]:
    old_path = root / "directui"
    new_path = root / "directuiold"

    changes: list[Change] = []

    if not old_path.exists():
        log("kromakit/ not found, skipping rename")
        return changes

    if not old_path.is_dir():
        die("directui exists but is not a directory")

    if new_path.exists():
        die("directuiold/ already exists; refusing to overwrite it")

    changes.append(Change(old_path, "rename kromakit/ to directuiold/"))

    log(f"{'rename' if apply else 'would rename'} kromakit/ -> directuiold/")

    if apply:
        old_path.rename(new_path)

    return changes


def cmake_quote(path: str) -> str:
    return path.replace("\\", "/")


def make_relative_or_abs(path: Path, root: Path) -> str:
    try:
        return path.resolve().relative_to(root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def generate_cmakelists(
    root: Path,
    kromakit_root: str,
    kromakit_build_dir: str,
    output_name: str,
) -> str:
    return f'''cmake_minimum_required(VERSION 3.25)

project(Synthem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(KROMAKIT_ROOT "{cmake_quote(kromakit_root)}" CACHE PATH "Path to KromaKit source tree")
set(KROMAKIT_BUILD_DIR "{cmake_quote(kromakit_build_dir)}" CACHE PATH "Path to KromaKit build directory")
set(KROMAKIT_STATIC_LIBRARY "${{KROMAKIT_BUILD_DIR}}/libkromakit.a" CACHE FILEPATH "Path to libkromakit.a")

if(NOT EXISTS "${{KROMAKIT_ROOT}}/include/kromakit")
    message(FATAL_ERROR "KROMAKIT_ROOT does not contain include/kromakit: ${{KROMAKIT_ROOT}}")
endif()

if(NOT EXISTS "${{KROMAKIT_STATIC_LIBRARY}}")
    message(FATAL_ERROR "Missing KromaKit static library: ${{KROMAKIT_STATIC_LIBRARY}}")
endif()

add_library(kromakit STATIC IMPORTED GLOBAL)

set_target_properties(kromakit PROPERTIES
    IMPORTED_LOCATION "${{KROMAKIT_STATIC_LIBRARY}}"
    INTERFACE_INCLUDE_DIRECTORIES "${{KROMAKIT_ROOT}}/include"
)

file(GLOB_RECURSE SYNTHEM_APP_SOURCES CONFIGURE_DEPENDS
    "${{CMAKE_CURRENT_SOURCE_DIR}}/synthem/*.cpp"
    "${{CMAKE_CURRENT_SOURCE_DIR}}/controls/*.cpp"
)

set(SYNTHEM_ROOT_SOURCES
    "${{CMAKE_CURRENT_SOURCE_DIR}}/main_linux.cpp"
)

add_executable(Synthem
    ${{SYNTHEM_ROOT_SOURCES}}
    ${{SYNTHEM_APP_SOURCES}}
)

target_include_directories(Synthem PRIVATE
    "${{CMAKE_CURRENT_SOURCE_DIR}}"
    "${{CMAKE_CURRENT_SOURCE_DIR}}/synthem"
    "${{CMAKE_CURRENT_SOURCE_DIR}}/controls"
)

target_compile_definitions(Synthem PRIVATE
    SYNTHEM_PLATFORM_LINUX=1
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW3 glfw3)
pkg_check_modules(FREETYPE2 freetype2)
pkg_check_modules(ALSA alsa)
pkg_check_modules(PIPEWIRE libpipewire-0.3)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(Synthem PRIVATE
    kromakit
    OpenGL::GL
    Threads::Threads
    dl
    m
)

if(GLFW3_FOUND)
    target_include_directories(Synthem PRIVATE ${{GLFW3_INCLUDE_DIRS}})
    target_link_libraries(Synthem PRIVATE ${{GLFW3_LIBRARIES}})
else()
    message(WARNING "glfw3 not found via pkg-config; KromaKit static link may fail")
endif()

if(FREETYPE2_FOUND)
    target_include_directories(Synthem PRIVATE ${{FREETYPE2_INCLUDE_DIRS}})
    target_link_libraries(Synthem PRIVATE ${{FREETYPE2_LIBRARIES}})
else()
    message(WARNING "freetype2 not found via pkg-config; KromaKit static link may fail")
endif()

if(ALSA_FOUND)
    target_include_directories(Synthem PRIVATE ${{ALSA_INCLUDE_DIRS}})
    target_link_libraries(Synthem PRIVATE ${{ALSA_LIBRARIES}})
endif()

if(PIPEWIRE_FOUND)
    target_include_directories(Synthem PRIVATE ${{PIPEWIRE_INCLUDE_DIRS}})
    target_link_libraries(Synthem PRIVATE ${{PIPEWIRE_LIBRARIES}})
endif()

add_custom_command(TARGET Synthem POST_BUILD
    COMMAND ${{CMAKE_COMMAND}} -E copy_directory
        "${{CMAKE_CURRENT_SOURCE_DIR}}/resources"
        "$<TARGET_FILE_DIR:Synthem>/resources"
    COMMENT "Copying Synthem resources"
)

set_target_properties(Synthem PROPERTIES
    OUTPUT_NAME "{output_name}"
)
'''


def write_cmakelists(root: Path, text: str, apply: bool, overwrite: bool) -> None:
    path = root / "CMakeLists.txt"

    if path.exists() and not overwrite:
        die("CMakeLists.txt already exists; pass --overwrite-cmake to replace it")

    log(f"{'write' if apply else 'would write'} CMakeLists.txt")

    if apply:
        path.write_text(text, encoding="utf-8")


def validate_root(root: Path) -> None:
    if not root.exists():
        die(f"project root does not exist: {root}")

    if not root.is_dir():
        die(f"project root is not a directory: {root}")

    if not (root / "synthem").exists():
        die("expected synthem/ directory at project root")

    if not (root / "resources").exists():
        log("warning: resources/ not found at project root")

    if not (root / "main_linux.cpp").exists():
        log("warning: main_linux.cpp not found at project root")


def default_kromakit_root(root: Path) -> str:
    sibling = root.parent / "KromaKit"
    return sibling.as_posix()


def default_kromakit_build_dir(kromakit_root: str) -> str:
    return f"{kromakit_root.rstrip('/')}/cmake-build-debug"


def print_next_steps(kromakit_root: str, kromakit_build_dir: str) -> None:
    log("")
    log("Next:")
    log("  # Make sure KromaKit is built first:")
    log(f"  cmake -S {kromakit_root} -B {kromakit_build_dir} -G Ninja")
    log(f"  cmake --build {kromakit_build_dir} -j 24")
    log("")
    log("  # Then build Synthem:")
    log("  cmake -S . -B out/synthem-linux -G Ninja \\")
    log(f"    -DKROMAKIT_ROOT={kromakit_root} \\")
    log(f"    -DKROMAKIT_BUILD_DIR={kromakit_build_dir}")
    log("  cmake --build out/synthem-linux -j 24")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Migrate Synthem to use KromaKit as a static library dependency."
    )

    parser.add_argument(
        "--project-root",
        default=".",
        help="Path to cloned Synthem project root. Default: current directory.",
    )

    parser.add_argument(
        "--kromakit-root",
        default=None,
        help="Path to KromaKit source root. Default: ../KromaKit relative to project root.",
    )

    parser.add_argument(
        "--kromakit-build-dir",
        default=None,
        help="Path to KromaKit CMake build dir containing libkromakit.a. Default: <kromakit-root>/cmake-build-debug.",
    )

    parser.add_argument(
        "--output-name",
        default="Synthem",
        help="Output executable name. Default: Synthem.",
    )

    parser.add_argument(
        "--apply",
        action="store_true",
        help="Actually write changes. Without this, the script is dry-run only.",
    )

    parser.add_argument(
        "--no-rename-directui",
        action="store_true",
        help="Do not rename kromakit/ to directuiold/.",
    )

    parser.add_argument(
        "--no-rewrite-includes",
        action="store_true",
        help="Do not rewrite kromakit/... path references.",
    )

    parser.add_argument(
        "--no-cmake",
        action="store_true",
        help="Do not generate CMakeLists.txt.",
    )

    parser.add_argument(
        "--overwrite-cmake",
        action="store_true",
        help="Overwrite existing CMakeLists.txt.",
    )

    args = parser.parse_args()

    root = Path(args.project_root).resolve()
    validate_root(root)

    kromakit_root = args.kromakit_root or default_kromakit_root(root)
    kromakit_build_dir = args.kromakit_build_dir or default_kromakit_build_dir(kromakit_root)

    log("Synthem -> KromaKit migration")
    log(f"  project root:        {root}")
    log(f"  KromaKit root:       {kromakit_root}")
    log(f"  KromaKit build dir:  {kromakit_build_dir}")
    log(f"  mode:                {'APPLY' if args.apply else 'dry-run'}")
    log("")

    if not args.no_rename_directui:
        rename_directui(root, args.apply)

    if not args.no_rewrite_includes:
        result = rewrite_files(root, args.apply)
        log("")
        log("Rewrite summary:")
        log(f"  files scanned:       {result.files_seen}")
        log(f"  files changed:       {result.files_changed}")
        log(f"  replacements:        {result.replacements}")

    if not args.no_cmake:
        cmake_text = generate_cmakelists(
            root=root,
            kromakit_root=kromakit_root,
            kromakit_build_dir=kromakit_build_dir,
            output_name=args.output_name,
        )
        write_cmakelists(root, cmake_text, args.apply, args.overwrite_cmake)

    if not args.apply:
        log("")
        log("Dry run only. Re-run with --apply to write changes.")
    else:
        print_next_steps(kromakit_root, kromakit_build_dir)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
