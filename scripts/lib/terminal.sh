#!/usr/bin/env bash

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

  C_ORANGE=$'\033[38;5;208m'
  C_PINK=$'\033[38;5;205m'
  C_PURPLE=$'\033[38;5;141m'
  C_GRAY=$'\033[38;5;245m'
  C_DARK=$'\033[38;5;238m'
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

  C_ORANGE=""
  C_PINK=""
  C_PURPLE=""
  C_GRAY=""
  C_DARK=""
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