#!/usr/bin/env bash

TARGET="${1:-.}"

tree -a "$TARGET" \
  -I 'build|build_package|resources|*.o|*.d|*.ipa|*.rawexec|*.app|Payload|_CodeSignature|CodeResources|compile_commands.json|.git|.idea|cmake-build-*|CMakeFiles|CMakeCache.txt|*.plist|*.ttf|*.png'
