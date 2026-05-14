#!/usr/bin/env bash
#set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SYNTHEM="$PROJECT_ROOT/synthem.sh"

[[ -x "$SYNTHEM" ]] || {
  echo "missing executable: $SYNTHEM" >&2
  exit 1
}

print_help() {
  cat <<'EOF'
Synthem command shell

Type commands without ./synthem.sh:

  build ios
  release all --clean -j 32
  package all
  zip all
  icons ios --theme studio
  logs ios
  help
  exit

Shortcuts:
  rls        -> release all --clean -j 32
  ios        -> dev ios
  ship       -> release all --clean -j 32 && zip all

EOF
}

is_synthem_command() {
  case "$1" in
    help|build|package|dev|release|clean|icons|plist|logs|deploy|zip)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

run_synthem() {
  echo
  "$SYNTHEM" "$@"
  local status=$?
  echo

  if [[ "$status" -ne 0 ]]; then
    echo "[synthem-shell] synthem command failed with exit code $status"
  fi

  return 0
}

run_bash_command() {
  bash -lc "$1"
  local status=$?

  if [[ "$status" -ne 0 ]]; then
    echo "[synthem-shell] bash command failed with exit code $status"
  fi

  return 0
}

prompt_dir() {
  if [[ "$PWD" == "$PROJECT_ROOT" ]]; then
    printf "~"
  elif [[ "$PWD" == "$PROJECT_ROOT/"* ]]; then
    printf "~/%s" "${PWD#"$PROJECT_ROOT"/}"
  else
    basename "$PWD"
  fi
}

handle_ctrl_c() {
  echo
  echo "type exit to exit"
}

trap handle_ctrl_c INT

print_help

while true; do
  if ! IFS= read -e -r -p "synthem $(prompt_dir) > " line; then
    echo
    continue
  fi

  [[ -z "${line//[[:space:]]/}" ]] && continue
  [[ "$line" == $'\e' || "$line" == $'\e['* ]] && continue

  case "$line" in
    exit|quit|q)
      break
      ;;

    clear|cls)
      clear
      ;;

    rls)
      run_synthem release all --clean -j 32
      ;;

    ios)
      run_synthem dev ios
      ;;

    ship)
      run_synthem release all --clean -j 32
      run_synthem zip all
      ;;

    cd|cd\ *)
      target_dir="${line#cd}"
      target_dir="${target_dir#"${target_dir%%[![:space:]]*}"}"
      if [[ -z "$target_dir" ]]; then
        cd "$PROJECT_ROOT" || true
      else
        cd "$target_dir" || echo "[synthem-shell] cd failed: $target_dir"
      fi
      ;;

    *)
      first_word="${line%%[[:space:]]*}"

      if is_synthem_command "$first_word"; then
        # shellcheck disable=SC2206
        args=($line)
        run_synthem "${args[@]}"
      else
        run_bash_command "$line"
      fi
      ;;
  esac
done