#!/usr/bin/env bash

APP_NAME="${APP_NAME:-Synthem}"

BUILD_DIR_FULL_PACKAGE="${BUILD_DIR_FULL_PACKAGE:-$PROJECT_ROOT/full_package}"

BUILD_IOS_DIR="${BUILD_IOS_DIR:-build_ios}"
BUILD_MACOS_DIR="${BUILD_MACOS_DIR:-build_macos}"

BUILD_IOS_FULL_PACKAGE_DIR="${BUILD_IOS_FULL_PACKAGE_DIR:-$BUILD_DIR_FULL_PACKAGE/build_ios}"
BUILD_MACOS_FULL_PACKAGE_DIR="${BUILD_MACOS_FULL_PACKAGE_DIR:-$BUILD_DIR_FULL_PACKAGE/build_macos}"

APPARATUS_CLI_BIN="${APPARATUS_CLI_BIN:-/home/code/RiderProjects/apparatus/CLI/bin/Release/net10.0/CLI}"
APPARATUS_GUI_BIN="${APPARATUS_GUI_BIN:-/home/code/RiderProjects/apparatus/apparatus.Desktop/bin/Release/net10.0/apparatus.Desktop.dll}"

apparatus_bundle_path() {
  local target="$1"

  case "$target" in
    macos)
      printf '%s' "$BUILD_MACOS_FULL_PACKAGE_DIR/$APP_NAME.app"
      ;;
    ios)
      printf '%s' "$BUILD_IOS_FULL_PACKAGE_DIR/$APP_NAME.app"
      ;;
    *)
      return 1
      ;;
  esac
}

apparatus_print_json_summary() {
  local json_file="$1"

  APPARATUS_WRAP_WIDTH="${APPARATUS_WRAP_WIDTH:-92}" \
  APP_C_RESET="$C_RESET" \
  APP_C_DIM="$C_DIM" \
  APP_C_RED="$C_RED" \
  APP_C_GREEN="$C_GREEN" \
  APP_C_YELLOW="$C_YELLOW" \
  APP_C_BLUE="$C_BLUE" \
  APP_C_MAGENTA="$C_MAGENTA" \
  APP_C_CYAN="$C_CYAN" \
  APP_C_BOLD="$C_BOLD" \
  APP_C_ORANGE="$C_ORANGE" \
  APP_C_PINK="$C_PINK" \
  APP_C_PURPLE="$C_PURPLE" \
  APP_C_GRAY="$C_GRAY" \
  APP_C_DARK="$C_DARK" \
  python3 - "$json_file" <<'PY'
import json
import os
import sys
import textwrap

path = sys.argv[1]

R = os.environ.get("APP_C_RESET", "")
DIM = os.environ.get("APP_C_DIM", "")
RED = os.environ.get("APP_C_RED", "")
GREEN = os.environ.get("APP_C_GREEN", "")
YELLOW = os.environ.get("APP_C_YELLOW", "")
BLUE = os.environ.get("APP_C_BLUE", "")
MAGENTA = os.environ.get("APP_C_MAGENTA", "")
CYAN = os.environ.get("APP_C_CYAN", "")
BOLD = os.environ.get("APP_C_BOLD", "")
ORANGE = os.environ.get("APP_C_ORANGE", "")
PINK = os.environ.get("APP_C_PINK", "")
PURPLE = os.environ.get("APP_C_PURPLE", "")
GRAY = os.environ.get("APP_C_GRAY", "")
DARK = os.environ.get("APP_C_DARK", "")

WRAP_WIDTH = int(os.environ.get("APPARATUS_WRAP_WIDTH", "100"))

def visible_len(text):
    # Good enough because we only use this for plain prefixes before colour.
    return len(text)

def wrap_text(text, width=WRAP_WIDTH, initial="      ", subsequent="      "):
    text = str(text or "")
    return textwrap.fill(
        text,
        width=width,
        initial_indent=initial,
        subsequent_indent=subsequent,
        break_long_words=False,
        break_on_hyphens=False,
    )

def print_wrapped(text, initial="      ", subsequent="      ", colour=DIM, width=WRAP_WIDTH):
    wrapped = wrap_text(text, width=width, initial=initial, subsequent=subsequent)
    for line in wrapped.splitlines():
        print(c(line, colour))

def row_wrapped(key, value, key_colour=GRAY, value_colour=None, width=WRAP_WIDTH):
    value = str(value or "")
    prefix = f"  {key:<24} "
    continuation = " " * len(prefix)

    if not value:
        print(f"{c(prefix, key_colour)}")
        return

    wrapped = textwrap.fill(
        value,
        width=width,
        initial_indent=prefix,
        subsequent_indent=continuation,
        break_long_words=False,
        break_on_hyphens=False,
    )

    for i, line in enumerate(wrapped.splitlines()):
        if i == 0:
            raw_prefix = line[:len(prefix)]
            raw_value = line[len(prefix):]
            if value_colour:
                print(f"{c(raw_prefix, key_colour)}{c(raw_value, value_colour)}")
            else:
                print(f"{c(raw_prefix, key_colour)}{raw_value}")
        else:
            if value_colour:
                print(c(line, value_colour))
            else:
                print(line)

def c(text, colour):
    return f"{colour}{text}{R}"

def row(key, value, colour=GRAY):
    print(f"  {c(f'{key:<24}', colour)} {value}")

def hr():
    print(c("────────────────────────────────────────────────────────────", DARK))

def title(text):
    print()
    print(c(text, BOLD + PURPLE))
    hr()

def section(text):
    print()
    print(c(text, BOLD + CYAN))
    hr()

try:
    with open(path, "r", encoding="utf-8") as f:
        data = json.load(f)
except Exception as exc:
    print(f"  {c('!', YELLOW)} failed to parse Apparatus JSON: {exc}")
    raise SystemExit(0)

identity = data.get("Identity") or {}
exe = data.get("Executable") or {}
diagnostics = data.get("Diagnostics") or []
icon = data.get("AppIcon") or {}

severity_names = {
    0: "trace",
    1: "warn",
    2: "error",
    3: "ok",
}

severity_colours = {
    0: DIM,
    1: YELLOW,
    2: RED,
    3: GREEN,
}

severity_markers = {
    0: "·",
    1: "!",
    2: "✗",
    3: "✓",
}

title("Apparatus summary")
row_wrapped("bundle", data.get("BundlePath", ""), BLUE)
row_wrapped("display name", identity.get("DisplayName", ""), PINK)
row_wrapped("bundle id", identity.get("BundleIdentifier", ""), PURPLE)
row_wrapped("version", identity.get("Version", ""), GREEN)
row_wrapped("build", identity.get("Build", ""), GREEN)
row_wrapped("minimum system", identity.get("MinimumSystemVersion", ""), ORANGE)
row_wrapped("executable", identity.get("ExecutableName", ""), CYAN)
row_wrapped("archs", exe.get("ArchitectureDisplay", ", ".join(exe.get("Architectures") or [])), CYAN)
row_wrapped("icon", icon.get("Path", ""), BLUE)

counts = {}
for d in diagnostics:
    severity = d.get("Severity")
    counts[severity] = counts.get(severity, 0) + 1

diag_bits = []
for severity, count in sorted(counts.items(), key=lambda item: str(item[0])):
    name = severity_names.get(severity, str(severity))
    colour = severity_colours.get(severity, GRAY)
    diag_bits.append(c(f"{name}={count}", colour))

diag_plain_bits = []
for severity, count in sorted(counts.items(), key=lambda item: str(item[0])):
    name = severity_names.get(severity, str(severity))
    diag_plain_bits.append(f"{name}={count}")

row_wrapped("diagnostics", ", ".join(diag_plain_bits), GRAY)

section("Diagnostics")

def diagnostic_palette(severity):
    if severity == 3:
        return {
            "marker": GREEN,
            "tag": GREEN,
            "message": BOLD + GREEN,
            "detail": DIM,
        }
    if severity == 2:
        return {
            "marker": RED,
            "tag": RED,
            "message": BOLD + RED,
            "detail": RED,
        }
    if severity == 1:
        return {
            "marker": YELLOW,
            "tag": YELLOW,
            "message": BOLD + YELLOW,
            "detail": DIM,
        }

    return {
        "marker": DARK,
        "tag": GRAY,
        "message": BOLD,
        "detail": DIM,
    }

def print_diagnostic(severity, name, message, detail):
    palette = diagnostic_palette(severity)
    marker = severity_markers.get(severity, "?")

    prefix_plain = f"  {marker} [{name}] "
    continuation_plain = " " * len(prefix_plain)

    wrapped_message = textwrap.fill(
        str(message or ""),
        width=WRAP_WIDTH,
        initial_indent=prefix_plain,
        subsequent_indent=continuation_plain,
        break_long_words=False,
        break_on_hyphens=False,
    )

    lines = wrapped_message.splitlines() or [prefix_plain]

    for i, line in enumerate(lines):
        if i == 0:
            prefix = line[:len(prefix_plain)]
            text = line[len(prefix_plain):]

            # Split prefix into marker and tag so both can have proper colour.
            marker_part = f"  {marker}"
            tag_part = f" [{name}] "

            print(
                f"{c(marker_part, palette['marker'])}"
                f"{c(tag_part, palette['tag'])}"
                f"{c(text, palette['message'])}"
            )
        else:
            text = line[len(continuation_plain):]
            print(
                f"{continuation_plain}"
                f"{c(text, palette['message'])}"
            )

    if detail:
        wrapped_detail = textwrap.fill(
            str(detail),
            width=WRAP_WIDTH,
            initial_indent="      ",
            subsequent_indent="      ",
            break_long_words=False,
            break_on_hyphens=False,
        )

        for line in wrapped_detail.splitlines():
            indent = line[:6]
            text = line[6:]
            print(f"{indent}{c(text, palette['detail'])}")

for d in diagnostics:
    severity = d.get("Severity")
    name = severity_names.get(severity, str(severity))
    message = d.get("Message", "")
    detail = d.get("Detail", "")

    print_diagnostic(severity, name, message, detail)
PY
}

gui() {
  run dotnet "$APPARATUS_GUI_BIN"

}

apparatus_input_path() {
  local target="$1"

  case "$target" in
    ios)
      if [[ -n "${OVERRIDE_IPA:-}" ]]; then
        printf '%s' "$OVERRIDE_IPA"
        return
      fi

      if [[ -n "${OVERRIDE_APP:-}" ]]; then
        printf '%s' "$OVERRIDE_APP"
        return
      fi

      apparatus_bundle_path ios
      ;;

    macos)
      if [[ -n "${OVERRIDE_IPA:-}" ]]; then
        die "apparatus --ipa only makes sense for ios, not macos"
      fi

      if [[ -n "${OVERRIDE_APP:-}" ]]; then
        printf '%s' "$OVERRIDE_APP"
        return
      fi

      apparatus_bundle_path macos
      ;;

    *)
      apparatus_bundle_path "$target"
      ;;
  esac
}

cmd_apparatus() {
  local target="all"
  local mode="summary"
  local save_json="0"
  local raw_json="0"
  local gui_mode="0"
  local out_dir="$PROJECT_ROOT/build_apparatus"
  local arg

  while (($#)); do
    arg="$1"
    shift

    case "$arg" in
      --json)
        raw_json="1"
        ;;
      --save-json)
        save_json="1"
        ;;
      --summary)
        mode="summary"
        ;;
      --raw)
        raw_json="1"
        ;;
      --out-dir)
        [[ $# -gt 0 ]] || die "apparatus --out-dir requires a path"
        out_dir="$1"
        shift
        ;;
      --gui)
        gui_mode="1"
        ;;
      all|macos|ios)
        target="$arg"
        ;;
      --ipa)
        [[ $# -gt 0 ]] || die "apparatus --ipa requires a path"
        OVERRIDE_IPA="$1"
        target="ios"
        shift
        ;;

      --ipa=*)
        OVERRIDE_IPA="${arg#*=}"
        target="ios"
        ;;

      --app)
        [[ $# -gt 0 ]] || die "apparatus --app requires a path"
        OVERRIDE_APP="$1"
        shift
        ;;

      --app=*)
        OVERRIDE_APP="${arg#*=}"
        ;;
      --*)
        die "unknown apparatus option: $arg"
        ;;
      *)
        die "unknown apparatus target: $arg (expected all, macos, ios)"
        ;;
    esac
  done

  if [[ "$gui_mode" == "1" ]]; then
    gui
    return
  fi

  [[ -x "$APPARATUS_CLI_BIN" ]] ||
    die "missing Apparatus CLI: $APPARATUS_CLI_BIN"

  command -v python3 >/dev/null 2>&1 ||
    die "apparatus summary requires python3"

  run mkdir -p "$out_dir"

  local targets=()
  case "$target" in
    all)
      targets=(macos ios)
      ;;
    macos|ios)
      targets=("$target")
      ;;
    *)
      die "invalid apparatus target: $target (expected all, macos, ios)"
      ;;
  esac

  local t bundle json_file

  for t in "${targets[@]}"; do
    bundle="$(apparatus_input_path "$t")"
    json_file="$out_dir/$t-apparatus.json"

    [[ -e "$bundle" ]] ||
      die "missing $t input: $bundle (run package all first)"

    log "${C_PURPLE}apparatus${C_RESET} ${C_CYAN}inspect${C_RESET} ${C_ORANGE}$t${C_RESET}: ${C_BLUE}$bundle${C_RESET}"

    if [[ "$raw_json" == "1" ]]; then
      "$APPARATUS_CLI_BIN" inspect "$bundle" --json
    else
      "$APPARATUS_CLI_BIN" inspect "$bundle" --json > "$json_file"
      apparatus_print_json_summary "$json_file"

      if [[ "$save_json" == "1" ]]; then
        log "${C_PURPLE}apparatus json${C_RESET}: ${C_BLUE}$json_file${C_RESET}"
      else
        rm -f "$json_file"
      fi
    fi
  done
}
