#!/usr/bin/env bash
# Synthem command shell.
#
# Keep this shell alive even when commands fail.
# Do not use set -e here.
set -uo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SYNTHEM="$PROJECT_ROOT/synthem.sh"

DEFAULT_JOBS="${SYNTHEM_SHELL_JOBS:-32}"

STATE_DIR="${XDG_STATE_HOME:-$HOME/.local/state}/synthem"
HISTFILE="${SYNTHEM_SHELL_HISTFILE:-$STATE_DIR/shell_history}"
LOG_DIR="${SYNTHEM_SHELL_LOG_DIR:-$STATE_DIR/logs}"

HISTSIZE="${SYNTHEM_SHELL_HISTSIZE:-5000}"
HISTFILESIZE="${SYNTHEM_SHELL_HISTFILESIZE:-20000}"
HISTCONTROL="ignoredups:erasedups"

LAST_STATUS=0
LAST_DURATION="0s"
LAST_COMMAND=""

PARSED_ARGS=()

COMMANDS=(
  help build package dev release clean icons plist logs deploy zip doctor inspect apparatus codesign zsign
)

PLATFORMS=(
  all ios macos linux windows
)

COMMON_FLAGS=(
  --clean --release --debug --verbose --theme --help
  -j --jobs
)

SHORTCUTS=(
  rls ios ship pkg zall ball mall wall lall
)

BUILTINS=(
  help exit quit q clear cls cd root pwd history shell source env which
)

mkdir -p "$STATE_DIR" "$LOG_DIR"
touch "$HISTFILE"

[[ -x "$SYNTHEM" ]] || {
  echo "missing executable: $SYNTHEM" >&2
  exit 1
}

set -o history
shopt -s histappend
shopt -s checkwinsize

history -r "$HISTFILE" 2>/dev/null || true

use_color() {
  [[ -t 1 && -z "${NO_COLOR:-}" ]]
}

if use_color; then
  C_RESET=$'\033[0m'
  C_DIM=$'\033[2m'
  C_RED=$'\033[31m'
  C_GREEN=$'\033[32m'
  C_YELLOW=$'\033[33m'
  C_BLUE=$'\033[34m'
  C_MAGENTA=$'\033[35m'
  C_CYAN=$'\033[36m'
  C_BOLD=$'\033[1m'
else
  C_RESET=""
  C_DIM=""
  C_RED=""
  C_GREEN=""
  C_YELLOW=""
  C_BLUE=""
  C_MAGENTA=""
  C_CYAN=""
  C_BOLD=""
fi

# Readline prompt markers.
# These tell readline that ANSI escapes do not take screen columns.
RL_START=$'\001'
RL_END=$'\002'

prompt_colour() {
  printf '%s%s%s' "$RL_START" "$1" "$RL_END"
}

if use_color; then
  C_ORANGE=$'\033[38;5;208m'
  C_PINK=$'\033[38;5;205m'
  C_PURPLE=$'\033[38;5;141m'
  C_GRAY=$'\033[38;5;245m'
  C_DARK=$'\033[38;5;238m'

  P_RESET="$(prompt_colour "$C_RESET")"
  P_DIM="$(prompt_colour "$C_DIM")"
  P_RED="$(prompt_colour "$C_RED")"
  P_GREEN="$(prompt_colour "$C_GREEN")"
  P_YELLOW="$(prompt_colour "$C_YELLOW")"
  P_BLUE="$(prompt_colour "$C_BLUE")"
  P_MAGENTA="$(prompt_colour "$C_MAGENTA")"
  P_CYAN="$(prompt_colour "$C_CYAN")"
  P_BOLD="$(prompt_colour "$C_BOLD")"
  P_ORANGE="$(prompt_colour "$C_ORANGE")"
  P_PINK="$(prompt_colour "$C_PINK")"
  P_PURPLE="$(prompt_colour "$C_PURPLE")"
  P_GRAY="$(prompt_colour "$C_GRAY")"
  P_DARK="$(prompt_colour "$C_DARK")"
else
  C_ORANGE=""
  C_PINK=""
  C_PURPLE=""
  C_GRAY=""
  C_DARK=""

  P_RESET=""
  P_DIM=""
  P_RED=""
  P_GREEN=""
  P_YELLOW=""
  P_BLUE=""
  P_MAGENTA=""
  P_CYAN=""
  P_BOLD=""
  P_ORANGE=""
  P_PINK=""
  P_PURPLE=""
  P_GRAY=""
  P_DARK=""
fi

say() {
  printf '%s\n' "$*"
}

info() {
  printf '%s%s%s\n' "$C_CYAN" "$*" "$C_RESET"
}

ok() {
  printf '%s%s%s\n' "$C_GREEN" "$*" "$C_RESET"
}

warn() {
  printf '%s%s%s\n' "$C_YELLOW" "$*" "$C_RESET" >&2
}

err() {
  printf '%s%s%s\n' "$C_RED" "$*" "$C_RESET" >&2
}

format_duration() {
  local seconds="$1"

  if ((seconds < 60)); then
    printf '%ss' "$seconds"
  else
    printf '%sm%02ss' "$((seconds / 60))" "$((seconds % 60))"
  fi
}

prompt_dir() {
  if [[ "$PWD" == "$PROJECT_ROOT" ]]; then
    printf '~'
  elif [[ "$PWD" == "$PROJECT_ROOT/"* ]]; then
    printf '~/%s' "${PWD#"$PROJECT_ROOT"/}"
  else
    printf '%s' "$PWD"
  fi
}

prompt_status() {
  if [[ "$LAST_STATUS" -eq 0 ]]; then
    printf '%sok%s' "$P_GREEN" "$P_RESET"
  else
    printf '%sx%s%s' "$P_RED" "$LAST_STATUS" "$P_RESET"
  fi
}

prompt_arrow() {
  if [[ "$LAST_STATUS" -eq 0 ]]; then
    printf '%s❯%s' "$P_GREEN" "$P_RESET"
  else
    printf '%s❯%s' "$P_RED" "$P_RESET"
  fi
}

make_prompt() {
  local dir status arrow duration_colour

  dir="$(prompt_dir)"
  status="$(prompt_status)"
  arrow="$(prompt_arrow)"

  if [[ "$LAST_STATUS" -eq 0 ]]; then
    duration_colour="$P_CYAN"
  else
    duration_colour="$P_YELLOW"
  fi

  printf '%s%s%s %s%s%s %s[%s %s%s%s]%s %s ' \
    "$P_BOLD$P_PURPLE" "synthem" "$P_RESET" \
    "$P_BLUE" "$dir" "$P_RESET" \
    "$P_DARK" "$status" "$duration_colour" "$LAST_DURATION" "$P_DARK" "$P_RESET" \
    "$arrow"
}

print_help() {
  cat <<EOF
${C_BOLD}${C_PURPLE}╭─────────────────────────────────────────╮${C_RESET}
${C_BOLD}${C_PURPLE}│${C_RESET} ${C_BOLD}${C_PINK}Synthem command shell${C_RESET} ${C_DIM}· project cockpit${C_RESET} ${C_BOLD}${C_PURPLE}│${C_RESET}
${C_BOLD}${C_PURPLE}╰─────────────────────────────────────────╯${C_RESET}

${C_BOLD}${C_CYAN}Type commands without ./synthem.sh:${C_RESET}

  ${C_GREEN}build${C_RESET}   ${C_BLUE}ios${C_RESET}
  ${C_GREEN}release${C_RESET} ${C_BLUE}all${C_RESET} ${C_YELLOW}--clean${C_RESET} ${C_YELLOW}-j${C_RESET} ${C_ORANGE}$DEFAULT_JOBS${C_RESET}
  ${C_GREEN}package${C_RESET} ${C_BLUE}all${C_RESET}
  ${C_GREEN}zip${C_RESET}     ${C_BLUE}all${C_RESET}
  ${C_GREEN}icons${C_RESET}   ${C_BLUE}ios${C_RESET} ${C_YELLOW}--theme${C_RESET} ${C_ORANGE}studio${C_RESET}
  ${C_GREEN}logs${C_RESET}    ${C_BLUE}ios${C_RESET}

${C_BOLD}${C_CYAN}Shell niceties:${C_RESET}

  ${C_MAGENTA}↑ / ↓${C_RESET}        prefix-search command history
  ${C_MAGENTA}Ctrl-R${C_RESET}       reverse history search
  ${C_MAGENTA}Tab${C_RESET}          lightweight command/platform/path completion
  ${C_MAGENTA}Ctrl-L${C_RESET}       clear screen
  ${C_MAGENTA}Ctrl-D${C_RESET}       exit
  ${C_GREEN}cd${C_RESET} ${C_GRAY}<dir>${C_RESET}     change shell directory
  ${C_GREEN}root${C_RESET}         cd to project root
  ${C_GREEN}history${C_RESET}      show recent shell history
  ${C_GREEN}shell${C_RESET}        open an interactive bash subshell

${C_BOLD}${C_CYAN}Shortcuts:${C_RESET}

  ${C_ORANGE}rls${C_RESET}          release all --clean -j $DEFAULT_JOBS
  ${C_ORANGE}ios${C_RESET}          dev ios
  ${C_ORANGE}ship${C_RESET}         release all --clean -j $DEFAULT_JOBS && zip all
  ${C_ORANGE}pkg${C_RESET}          package all
  ${C_ORANGE}zall${C_RESET}         zip all
  ${C_ORANGE}ball${C_RESET}         build all
  ${C_ORANGE}lall${C_RESET}         release linux --clean -j $DEFAULT_JOBS
  ${C_ORANGE}mall${C_RESET}         release macos --clean -j $DEFAULT_JOBS
  ${C_ORANGE}wall${C_RESET}         release windows --clean -j $DEFAULT_JOBS

${C_DIM}${C_BOLD}Anything that is not a Synthem command or shell builtin is run through bash.${C_RESET}

EOF
}

is_synthem_command() {
  local word="${1:-}"

  case "$word" in
    build|package|dev|release|clean|icons|plist|logs|deploy|zip|doctor|inspect|apparatus|codesign|zsign)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

has_python3() {
  command -v python3 >/dev/null 2>&1
}

parse_args() {
  local line="$1"
  local parsed
  PARSED_ARGS=()

  if has_python3; then
    if ! parsed="$(
      python3 - "$line" <<'PY'
import shlex
import sys

line = sys.argv[1]

try:
    parts = shlex.split(line)
except ValueError as exc:
    print(exc, file=sys.stderr)
    sys.exit(2)

for part in parts:
    print(part)
PY
    )"; then
      err "[synthem-shell] parse error"
      return 2
    fi

    if [[ -n "$parsed" ]]; then
      mapfile -t PARSED_ARGS <<< "$parsed"
    fi
  else
    # Fallback: no quote handling, but still usable.
    # Install python3 if you want robust quoted args.
    read -r -a PARSED_ARGS <<< "$line"
  fi

  return 0
}

remember_history() {
  local line="$1"

  [[ -z "${line//[[:space:]]/}" ]] && return 0
  [[ "$line" == " "* ]] && return 0
  [[ "$line" == "$LAST_COMMAND" ]] && return 0
  [[ "$line" == "history clear" ]] && return 0

  history -s "$line"
  history -w "$HISTFILE" 2>/dev/null || true
  LAST_COMMAND="$line"
}

expand_cd_target() {
  local target="$1"

  case "$target" in
    "~")
      printf '%s' "$HOME"
      ;;
    "~/"*)
      printf '%s/%s' "$HOME" "${target#"~/"}"
      ;;
    *)
      printf '%s' "$target"
      ;;
  esac
}

run_with_timer() {
  local start end status

  start="$(date +%s)"
  "$@"
  status=$?
  end="$(date +%s)"

  LAST_STATUS="$status"
  LAST_DURATION="$(format_duration "$((end - start))")"

  return "$status"
}

run_synthem() {
  local status

  echo
  run_with_timer "$SYNTHEM" "$@"
  status=$?
  echo

  if [[ "$status" -eq 0 ]]; then
    ok "[synthem-shell] done in $LAST_DURATION"
  else
    err "[synthem-shell] synthem command failed with exit code $status after $LAST_DURATION"
  fi

  return "$status"
}

run_bash_command() {
  local line="$1"
  local status

  echo
  run_with_timer bash -lc "$line"
  status=$?
  echo

  if [[ "$status" -ne 0 ]]; then
    err "[synthem-shell] bash command failed with exit code $status after $LAST_DURATION"
  fi

  return "$status"
}

do_cd() {
  local target="${1:-$PROJECT_ROOT}"

  target="$(expand_cd_target "$target")"

  if cd "$target"; then
    LAST_STATUS=0
    LAST_DURATION="0s"
  else
    LAST_STATUS=1
    LAST_DURATION="0s"
    err "[synthem-shell] cd failed: $target"
  fi
}

show_history() {
  local count="${1:-40}"

  if [[ "$count" == "clear" ]]; then
    history -c
    : > "$HISTFILE"
    ok "[synthem-shell] history cleared"
    return 0
  fi

  history | tail -n "$count"
}

open_subshell() {
  info "[synthem-shell] entering bash subshell. Type exit to return."
  bash -i
}

common_prefix() {
  local prefix="$1"
  shift

  local item
  for item in "$@"; do
    while [[ "${item:0:${#prefix}}" != "$prefix" && -n "$prefix" ]]; do
      prefix="${prefix:0:${#prefix}-1}"
    done
  done

  printf '%s' "$prefix"
}

print_completion_list() {
  local -a items=("$@")
  local item

  printf '\n'
  for item in "${items[@]}"; do
    case "$item" in
      all|ios|macos|linux|windows)
        printf '  %s%s%s\n' "$C_BLUE" "$item" "$C_RESET"
        ;;
      --*|-*)
        printf '  %s%s%s\n' "$C_YELLOW" "$item" "$C_RESET"
        ;;
      rls|ship|pkg|zall|ball|lall|mall|wall)
        printf '  %s%s%s\n' "$C_ORANGE" "$item" "$C_RESET"
        ;;
      */)
        printf '  %s%s%s\n' "$C_CYAN" "$item" "$C_RESET"
        ;;
      *)
        printf '  %s%s%s\n' "$C_GREEN" "$item" "$C_RESET"
        ;;
    esac
  done
  printf '\n'
}

complete_path_matches() {
  local word="$1"
  local -n out_ref="$2"
  local -a raw=()
  local item

  mapfile -t raw < <(compgen -f -- "$word" 2>/dev/null || true)

  out_ref=()
  for item in "${raw[@]}"; do
    if [[ -d "$item" ]]; then
      out_ref+=("$item/")
    else
      out_ref+=("$item")
    fi
  done
}

synthem_complete_line() {
  local line="${READLINE_LINE:-}"
  local point="${READLINE_POINT:-0}"
  local before="${line:0:point}"
  local after="${line:point}"
  local word=""
  local start="$point"
  local first=""
  local -a candidates=()
  local -a matches=()
  local candidate
  local replacement

  if [[ "$before" =~ ([^[:space:]]+)$ ]]; then
    word="${BASH_REMATCH[1]}"
    start=$((point - ${#word}))
  fi

  first="${line%%[[:space:]]*}"

  if [[ "$start" -eq 0 ]]; then
    candidates=("${BUILTINS[@]}" "${SHORTCUTS[@]}" "${COMMANDS[@]}")
  elif [[ "$first" == "cd" ]]; then
    complete_path_matches "$word" candidates
  elif [[ "$word" == */* || "$word" == .* || "$word" == "~"* ]]; then
    complete_path_matches "$word" candidates
  elif is_synthem_command "$first"; then
    candidates=("${PLATFORMS[@]}" "${COMMON_FLAGS[@]}")
  else
    complete_path_matches "$word" candidates
    candidates+=("${PLATFORMS[@]}" "${COMMON_FLAGS[@]}")
  fi

  for candidate in "${candidates[@]}"; do
    if [[ "$candidate" == "$word"* ]]; then
      matches+=("$candidate")
    fi
  done

  if ((${#matches[@]} == 0)); then
    return 0
  fi

  if ((${#matches[@]} == 1)); then
    replacement="${matches[0]}"
    READLINE_LINE="${line:0:start}${replacement}${after}"
    READLINE_POINT=$((start + ${#replacement}))
    return 0
  fi

  replacement="$(common_prefix "${matches[0]}" "${matches[@]}")"

  if [[ "$replacement" != "$word" ]]; then
    READLINE_LINE="${line:0:start}${replacement}${after}"
    READLINE_POINT=$((start + ${#replacement}))
    return 0
  fi

  print_completion_list "${matches[@]}"
  return 0
}

setup_readline() {
  bind 'set completion-ignore-case on' 2>/dev/null || true
  bind 'set show-all-if-ambiguous on' 2>/dev/null || true
  bind 'set colored-stats on' 2>/dev/null || true
  bind 'set mark-symlinked-directories on' 2>/dev/null || true

  # Prefix history search. Type "pack", press up, get previous package commands.
  bind '"\e[A": history-search-backward' 2>/dev/null || true
  bind '"\e[B": history-search-forward' 2>/dev/null || true

  # Ctrl-L clear, Ctrl-R reverse search are already Readline classics.
  bind -x '"\t": synthem_complete_line' 2>/dev/null || true
}

read_shell_line() {
  local prompt="$1"
  local line

  if [[ -t 0 ]]; then
    if IFS= read -e -r -p "$prompt" line; then
      printf '%s' "$line"
      return 0
    fi
  else
    if IFS= read -r line; then
      printf '%s' "$line"
      return 0
    fi
  fi

  return 1
}

dispatch_line() {
  local line="$1"
  local cmd=""

  [[ -z "${line//[[:space:]]/}" ]] && return 0
  [[ "$line" == $'\e' || "$line" == $'\e['* ]] && return 0
  [[ "$line" == \#* ]] && return 0

  parse_args "$line" || {
    LAST_STATUS=2
    LAST_DURATION="0s"
    return 2
  }

  ((${#PARSED_ARGS[@]} == 0)) && return 0
  cmd="${PARSED_ARGS[0]}"

  case "$cmd" in
    exit|quit|q)
      return 99
      ;;

    help|\?)
      print_help
      LAST_STATUS=0
      LAST_DURATION="0s"
      ;;

    clear|cls)
      clear
      LAST_STATUS=0
      LAST_DURATION="0s"
      ;;

    cd)
      if ((${#PARSED_ARGS[@]} > 2)); then
        err "[synthem-shell] cd accepts one path"
        LAST_STATUS=2
        LAST_DURATION="0s"
      else
        do_cd "${PARSED_ARGS[1]:-}"
      fi
      ;;

    root)
      do_cd "$PROJECT_ROOT"
      ;;

    pwd)
      pwd
      LAST_STATUS=0
      LAST_DURATION="0s"
      ;;

    history)
      show_history "${PARSED_ARGS[1]:-40}"
      LAST_STATUS=0
      LAST_DURATION="0s"
      ;;

    shell)
      run_with_timer open_subshell
      ;;

    synthem)
      if ((${#PARSED_ARGS[@]} == 1)); then
        run_synthem help
      else
        run_synthem "${PARSED_ARGS[@]:1}"
      fi
      ;;

    rls)
      run_synthem release all --clean -j "$DEFAULT_JOBS"
      ;;

    ios)
      run_synthem dev ios
      ;;

    ship)
      run_synthem release all --clean -j "$DEFAULT_JOBS" && run_synthem zip all
      ;;

    pkg)
      run_synthem package all
      ;;

    zall)
      run_synthem zip all
      ;;

    ball)
      run_synthem build all
      ;;

    lall)
      run_synthem release linux --clean -j "$DEFAULT_JOBS"
      ;;

    mall)
      run_synthem release macos --clean -j "$DEFAULT_JOBS"
      ;;

    wall)
      run_synthem release windows --clean -j "$DEFAULT_JOBS"
      ;;

    *)
      if is_synthem_command "$cmd"; then
        run_synthem "${PARSED_ARGS[@]}"
      else
        run_bash_command "$line"
      fi
      ;;
  esac
}

handle_ctrl_c() {
  echo
  warn "type exit to exit"
  LAST_STATUS=130
  LAST_DURATION="0s"
}

cleanup_shell() {
  history -w "$HISTFILE" 2>/dev/null || true
}

trap handle_ctrl_c INT
trap cleanup_shell EXIT

setup_readline
print_help

while true; do
  line="$(read_shell_line "$(make_prompt)")" || {
    echo
    break
  }

  if [[ -z "${line//[[:space:]]/}" ]]; then
    continue
  fi

  if [[ "$line" != "history clear" ]]; then
    remember_history "$line"
  fi

  dispatch_line "$line"
  dispatch_status=$?

  if [[ "$dispatch_status" -eq 99 ]]; then
    break
  fi
done
