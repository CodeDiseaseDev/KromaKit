#!/usr/bin/env bash
set -euo pipefail

# scripts/commands/zsign.sh
# Intended to be sourced by synthem.sh, but can also run standalone if needed.

ZSIGN_BINARY="${ZSIGN_BINARY:-/home/code/Coding/github-repos/zsign/bin/zsign}"

PROJECT_ROOT="${PROJECT_ROOT:-/home/code/Coding/SynthemKit}"
APP_NAME="${APP_NAME:-Synthem}"

MACOS_SIGNED_APP="${MACOS_SIGNED_APP:-$PROJECT_ROOT/full_package/build_macos/$APP_NAME-zsigned.app}"
MACOS_UNSIGNED_APP="${MACOS_UNSIGNED_APP:-$PROJECT_ROOT/full_package/build_macos/$APP_NAME.app}"

IOS_UNSIGNED_IPA="${IOS_UNSIGNED_IPA:-$PROJECT_ROOT/full_package/build_ios/$APP_NAME.ipa}"
IOS_SIGNED_IPA="${IOS_SIGNED_IPA:-$PROJECT_ROOT/full_package/build_ios/$APP_NAME-zsigned.ipa}"

P12_FILE="${P12_FILE:-$PROJECT_ROOT/signing/private/apple-dev.p12}"
LOCALMOBILEPROVISION="${LOCALMOBILEPROVISION:-$PROJECT_ROOT/signing/private/synthem.mobileprovision}"

# Colours, with fallback if your main terminal colours are not loaded.
C_RESET="${C_RESET:-$'\033[0m'}"
C_DIM="${C_DIM:-$'\033[2m'}"
C_RED="${C_RED:-$'\033[31m'}"
C_GREEN="${C_GREEN:-$'\033[32m'}"
C_YELLOW="${C_YELLOW:-$'\033[33m'}"
C_BLUE="${C_BLUE:-$'\033[34m'}"
C_MAGENTA="${C_MAGENTA:-$'\033[35m'}"
C_CYAN="${C_CYAN:-$'\033[36m'}"
C_BOLD="${C_BOLD:-$'\033[1m'}"

zsign_log() {
  if declare -F log >/dev/null 2>&1; then
    log "$@"
  else
    printf '%s[zsign]%s %s\n' "$C_CYAN" "$C_RESET" "$*"
  fi
}

zsign_warn() {
  if declare -F warn >/dev/null 2>&1; then
    warn "$@"
  else
    printf '%s[zsign][warn]%s %s\n' "$C_YELLOW" "$C_RESET" "$*" >&2
  fi
}

zsign_die() {
  if declare -F die >/dev/null 2>&1; then
    die "$@"
  else
    printf '%s[zsign][error]%s %s\n' "$C_RED" "$C_RESET" "$*" >&2
    exit 1
  fi
}

zsign_hr() {
  printf '%s────────────────────────────────────────────────────────────%s\n' "$C_DIM" "$C_RESET"
}

zsign_file_size() {
  local path="$1"

  if command -v du >/dev/null 2>&1; then
    du -h "$path" | awk '{print $1}'
  else
    printf '?'
  fi
}

zsign_prompt_password() {
  local pass_var_name="$1"
  local password=""

  if [[ -n "${ZSIGN_P12_PASSWORD:-}" ]]; then
    printf -v "$pass_var_name" '%s' "$ZSIGN_P12_PASSWORD"
    return 0
  fi

  printf '%sP12 password%s: ' "$C_MAGENTA" "$C_RESET" >&2
  IFS= read -r -s password
  printf '\n' >&2

  [[ -n "$password" ]] ||
    zsign_die "empty P12 password"

  printf -v "$pass_var_name" '%s' "$password"
}

zsign_check_ios_inputs() {
  [[ -x "$ZSIGN_BINARY" ]] ||
    zsign_die "missing or non-executable zsign binary: $ZSIGN_BINARY"

  [[ -f "$IOS_UNSIGNED_IPA" ]] ||
    zsign_die "missing unsigned iOS IPA: $IOS_UNSIGNED_IPA"

  [[ -f "$P12_FILE" ]] ||
    zsign_die "missing p12 certificate: $P12_FILE"

  [[ -f "$LOCALMOBILEPROVISION" ]] ||
    zsign_die "missing provisioning profile: $LOCALMOBILEPROVISION"

  mkdir -p "$(dirname "$IOS_SIGNED_IPA")"
}

cmd_zsign_ios() {
  local p12_password=""

  zsign_check_ios_inputs

  printf '\n%s%s · local iOS signing%s\n' "$C_BOLD" "$APP_NAME" "$C_RESET"
  zsign_hr
  printf '  %starget%s     %sios%s\n' \
    "$C_CYAN" "$C_RESET" "$C_GREEN" "$C_RESET"
  printf '  %sinput%s      %s%s%s %s(%s)%s\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$IOS_UNSIGNED_IPA" "$C_RESET" "$C_DIM" "$(zsign_file_size "$IOS_UNSIGNED_IPA")" "$C_RESET"
  printf '  %soutput%s     %s%s%s\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$IOS_SIGNED_IPA" "$C_RESET"
  printf '  %scert%s       %s%s%s\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$P12_FILE" "$C_RESET"
  printf '  %sprofile%s    %s%s%s\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$LOCALMOBILEPROVISION" "$C_RESET"
  printf '  %szsign%s      %s%s%s\n\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$ZSIGN_BINARY" "$C_RESET"

  zsign_prompt_password p12_password

  rm -f "$IOS_SIGNED_IPA"

  zsign_log "${C_MAGENTA}signing ios${C_RESET} ${C_BLUE}$IOS_UNSIGNED_IPA${C_RESET}"

  "$ZSIGN_BINARY" \
    -k "$P12_FILE" \
    -p "$p12_password" \
    -m "$LOCALMOBILEPROVISION" \
    -o "$IOS_SIGNED_IPA" \
    "$IOS_UNSIGNED_IPA"

  # Remove password from variable as soon as zsign returns.
  p12_password=""

  [[ -f "$IOS_SIGNED_IPA" ]] ||
    zsign_die "zsign did not create output IPA: $IOS_SIGNED_IPA"

  printf '\n'
  zsign_hr
  printf '  %s✓ signed iOS IPA%s  %s%s%s %s(%s)%s\n' \
    "$C_GREEN" "$C_RESET" "$C_BLUE" "$IOS_SIGNED_IPA" "$C_RESET" "$C_DIM" "$(zsign_file_size "$IOS_SIGNED_IPA")" "$C_RESET"

  if command -v sha256sum >/dev/null 2>&1; then
    local hash
    hash="$(sha256sum "$IOS_SIGNED_IPA" | awk '{print $1}')"
    printf '  %ssha256%s          %s%s%s\n' \
      "$C_CYAN" "$C_RESET" "$C_DIM" "$hash" "$C_RESET"
  fi

  printf '\n'
}

zsign_check_macos_inputs() {
  [[ -x "$ZSIGN_BINARY" ]] ||
    zsign_die "missing or non-executable zsign binary: $ZSIGN_BINARY"

  [[ -d "$MACOS_UNSIGNED_APP" ]] ||
    zsign_die "missing unsigned macOS app: $MACOS_UNSIGNED_APP"

  [[ -f "$P12_FILE" ]] ||
    zsign_die "missing p12 certificate: $P12_FILE"

  mkdir -p "$(dirname "$MACOS_SIGNED_APP")"
}

cmd_zsign_macos() {
  local p12_password=""

  zsign_check_macos_inputs

  printf '\n%s%s · local macOS signing%s\n' "$C_BOLD" "$APP_NAME" "$C_RESET"
  zsign_hr
  printf '  %starget%s     %smacos%s\n' \
    "$C_CYAN" "$C_RESET" "$C_GREEN" "$C_RESET"
  printf '  %sinput%s      %s%s%s %s(%s)%s\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$MACOS_UNSIGNED_APP" "$C_RESET" "$C_DIM" "$(zsign_file_size "$MACOS_UNSIGNED_APP")" "$C_RESET"
  printf '  %soutput%s     %s%s%s\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$MACOS_SIGNED_APP" "$C_RESET"
  printf '  %scert%s       %s%s%s\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$P12_FILE" "$C_RESET"
  printf '  %szsign%s      %s%s%s\n\n' \
    "$C_CYAN" "$C_RESET" "$C_BLUE" "$ZSIGN_BINARY" "$C_RESET"

  zsign_prompt_password p12_password

  rm -rf "$MACOS_SIGNED_APP"

  zsign_log "${C_MAGENTA}copying macOS app staging copy${C_RESET}"
  rsync -a --delete "$MACOS_UNSIGNED_APP/" "$MACOS_SIGNED_APP/"

  zsign_log "${C_MAGENTA}signing macos${C_RESET} ${C_BLUE}$MACOS_SIGNED_APP${C_RESET}"

  "$ZSIGN_BINARY" \
    -k "$P12_FILE" \
    -p "$p12_password" \
    "$MACOS_SIGNED_APP"

  p12_password=""

  [[ -d "$MACOS_SIGNED_APP" ]] ||
    zsign_die "zsign did not create/keep signed app: $MACOS_SIGNED_APP"

  printf '\n'
  zsign_hr
  printf '  %s✓ signed macOS app%s  %s%s%s %s(%s)%s\n' \
    "$C_GREEN" "$C_RESET" "$C_BLUE" "$MACOS_SIGNED_APP" "$C_RESET" "$C_DIM" "$(zsign_file_size "$MACOS_SIGNED_APP")" "$C_RESET"

  local executable="$MACOS_SIGNED_APP/Contents/MacOS/$APP_NAME"
  if [[ -f "$executable" ]]; then
    if command -v llvm-objdump >/dev/null 2>&1; then
      if llvm-objdump --macho --private-headers "$executable" 2>/dev/null | grep -q LC_CODE_SIGNATURE; then
        printf '  %s✓ code signature%s    LC_CODE_SIGNATURE found\n' "$C_GREEN" "$C_RESET"
      else
        printf '  %s! code signature%s    LC_CODE_SIGNATURE not found\n' "$C_YELLOW" "$C_RESET"
      fi
    fi
  fi

  printf '\n'
}

cmd_zsign() {
  local target="${1:-ios}"

  case "$target" in
    ios)
      cmd_zsign_ios
      ;;
    macos)
      cmd_zsign_macos
      ;;
    all)
      cmd_zsign_ios
      cmd_zsign_macos
      ;;
    -h|--help|help)
      cat <<EOF
Usage:
  zsign [ios|macos|all]

Targets:
  ios     sign full_package/build_ios/$APP_NAME.ipa using local zsign
  macos   sign full_package/build_macos/$APP_NAME.app using local zsign
  all     sign ios, then macos

Environment overrides:
  ZSIGN_BINARY
  IOS_UNSIGNED_IPA
  IOS_SIGNED_IPA
  P12_FILE
  LOCALMOBILEPROVISION
  ZSIGN_P12_PASSWORD
EOF
      ;;
    *)
      zsign_die "unknown zsign target: $target (expected ios, macos, all)"
      ;;
  esac
}