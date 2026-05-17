#!/usr/bin/env bash

# scripts/commands/mac_codesign.sh
#
# This file is intended to be sourced by synthem.sh.
# It must not execute signing work at source time.

# ─────────────────────────────────────────────────────────────
# Config defaults
# ─────────────────────────────────────────────────────────────

MAC_HOST="${MAC_HOST:-deacons-mbp}"

APP_NAME="${APP_NAME:-Synthem}"
BUNDLE_ID="${BUNDLE_ID:-com.codelmao.synthem}"

PROJECT_ROOT="${PROJECT_ROOT:-$(pwd)}"

LOCAL_APP_DIR="${LOCAL_APP_DIR:-$PROJECT_ROOT/full_package/build_ios/$APP_NAME.app}"
LOCAL_SIGNED_IPA="${LOCAL_SIGNED_IPA:-$PROJECT_ROOT/full_package/build_ios/$APP_NAME-signed.ipa}"

REMOTE_ROOT="${REMOTE_ROOT:-/tmp/loaderkit-sign-$APP_NAME}"
REMOTE_PAYLOAD_DIR="$REMOTE_ROOT/Payload"
REMOTE_APP_DIR="$REMOTE_PAYLOAD_DIR/$APP_NAME.app"
REMOTE_IPA="$REMOTE_ROOT/$APP_NAME-signed.ipa"
REMOTE_SCRIPT="$REMOTE_ROOT/sign-on-mac.sh"

# Optional overrides.
# Empty MOBILEPROVISION means the Mac-side script will auto-find one.
SIGN_IDENTITY="Apple Development: happyfone579@gmail.com (DRFXG2PL28)"
MOBILEPROVISION="/Users/code/Library/Developer/Xcode/UserData/Provisioning Profiles/3636d537-fb89-42c9-b350-5faccc8e3248.mobileprovision"
ENTITLEMENTS_FILE="${ENTITLEMENTS_FILE:-}"

mac_codesign_die() {
  if declare -F die >/dev/null 2>&1; then
    die "$@"
  fi

  echo "[sign-ios][error] $*" >&2
  exit 1
}

mac_codesign_log() {
  if declare -F log >/dev/null 2>&1; then
    log "$@"
  else
    echo "[sign-ios] $*"
  fi
}

generate_remote_mac_codesign_script() {
  local out="$1"

  cat > "$out" <<'REMOTE_SCRIPT_EOF'
#!/usr/bin/env bash
set -euo pipefail

APP_NAME="$1"
BUNDLE_ID="$2"
REMOTE_ROOT="$3"
REMOTE_APP_DIR="$4"
REMOTE_IPA="$5"
SIGN_IDENTITY="$6"
MOBILEPROVISION="$7"
ENTITLEMENTS_FILE="$8"

die() {
  echo "[mac-sign][error] $*" >&2
  exit 1
}

log() {
  echo "[mac-sign] $*"
}

unlock_keychain_if_possible() {
  security unlock-keychain "$HOME/Library/Keychains/login.keychain-db" >/dev/null 2>&1 || true
}

plist_read() {
  /usr/libexec/PlistBuddy -c "Print :$2" "$1" 2>/dev/null || true
}

decode_mobileprovision() {
  local profile="$1"
  local out="$2"

  security cms -D -i "$profile" > "$out"
}

find_identity() {
  if [[ -n "$SIGN_IDENTITY" ]]; then
    echo "$SIGN_IDENTITY"
    return 0
  fi

  local identity
  identity="$(
    security find-identity -v -p codesigning |
      sed -n 's/.*"\(Apple Development: .*\)".*/\1/p' |
      head -n 1
  )"

  [[ -n "$identity" ]] ||
    die "could not find an Apple Development signing identity in keychain"

  echo "$identity"
}

find_profile_for_bundle() {
  local wanted_bundle="$1"

  local profile_dirs=(
    "$HOME/Library/MobileDevice/Provisioning Profiles"
    "$HOME/Library/Developer/Xcode/UserData/Provisioning Profiles"
  )

  local profile decoded entitlements_app_id suffix best_profile=""

  for profiles_dir in "${profile_dirs[@]}"; do
    [[ -d "$profiles_dir" ]] || continue

    while IFS= read -r -d '' profile; do
      decoded="$REMOTE_ROOT/profile-decoded.plist"

      if ! decode_mobileprovision "$profile" "$decoded" >/dev/null 2>&1; then
        continue
      fi

      entitlements_app_id="$(
        /usr/libexec/PlistBuddy -c "Print :Entitlements:application-identifier" "$decoded" 2>/dev/null || true
      )"

      [[ -n "$entitlements_app_id" ]] || continue

      # application-identifier normally looks like TEAMID.com.example.app
      suffix="${entitlements_app_id#*.}"

      if [[ "$suffix" == "$wanted_bundle" ]]; then
        best_profile="$profile"
        break 2
      fi

      # Basic wildcard support, for example TEAMID.com.example.*
      if [[ "$suffix" == *"*" ]]; then
        local prefix="${suffix%\*}"
        if [[ "$wanted_bundle" == "$prefix"* ]]; then
          best_profile="$profile"
          break 2
        fi
      fi
    done < <(find "$profiles_dir" -name "*.mobileprovision" -type f -print0)
  done

  [[ -n "$best_profile" ]] ||
    die "could not find provisioning profile matching bundle id: $wanted_bundle"

  echo "$best_profile"
}

make_entitlements_from_profile() {
  local profile="$1"
  local out="$2"
  local decoded="$REMOTE_ROOT/profile.plist"

  decode_mobileprovision "$profile" "$decoded"

  /usr/libexec/PlistBuddy -x -c "Print :Entitlements" "$decoded" > "$out"
}

print_profile_summary() {
  local profile="$1"
  local decoded="$REMOTE_ROOT/profile-summary.plist"

  decode_mobileprovision "$profile" "$decoded"

  log "profile name: $(/usr/libexec/PlistBuddy -c "Print :Name" "$decoded" 2>/dev/null || true)"
  log "profile uuid: $(/usr/libexec/PlistBuddy -c "Print :UUID" "$decoded" 2>/dev/null || true)"
  log "profile app id: $(/usr/libexec/PlistBuddy -c "Print :Entitlements:application-identifier" "$decoded" 2>/dev/null || true)"
  log "profile team: $(/usr/libexec/PlistBuddy -c "Print :Entitlements:com.apple.developer.team-identifier" "$decoded" 2>/dev/null || true)"
  log "profile expires: $(/usr/libexec/PlistBuddy -c "Print :ExpirationDate" "$decoded" 2>/dev/null || true)"
}

[[ -d "$REMOTE_APP_DIR" ]] ||
  die "missing remote app bundle: $REMOTE_APP_DIR"

[[ -f "$REMOTE_APP_DIR/Info.plist" ]] ||
  die "missing Info.plist inside app"

actual_bundle="$(plist_read "$REMOTE_APP_DIR/Info.plist" CFBundleIdentifier)"
[[ "$actual_bundle" == "$BUNDLE_ID" ]] ||
  die "bundle id mismatch. expected '$BUNDLE_ID', app has '$actual_bundle'"

identity="$(find_identity)"
log "identity: $identity"

if [[ -z "$MOBILEPROVISION" ]]; then
  MOBILEPROVISION="$(find_profile_for_bundle "$BUNDLE_ID")"
fi

[[ -f "$MOBILEPROVISION" ]] ||
  die "missing provisioning profile: $MOBILEPROVISION"

log "profile: $MOBILEPROVISION"
print_profile_summary "$MOBILEPROVISION"

cp "$MOBILEPROVISION" "$REMOTE_APP_DIR/embedded.mobileprovision"

if [[ -z "$ENTITLEMENTS_FILE" ]]; then
  ENTITLEMENTS_FILE="$REMOTE_ROOT/entitlements.plist"
  make_entitlements_from_profile "$MOBILEPROVISION" "$ENTITLEMENTS_FILE"
fi

[[ -f "$ENTITLEMENTS_FILE" ]] ||
  die "missing entitlements file: $ENTITLEMENTS_FILE"

log "entitlements: $ENTITLEMENTS_FILE"
/usr/libexec/PlistBuddy -c "Print" "$ENTITLEMENTS_FILE" || true

# Remove old signature bits first.
rm -rf "$REMOTE_APP_DIR/_CodeSignature"

# Sign nested code first if any exists.
while IFS= read -r -d '' nested; do
  case "$nested" in
    "$REMOTE_APP_DIR")
      ;;
    *)
      log "signing nested code: $nested"
      codesign --force \
        --sign "$identity" \
        --entitlements "$ENTITLEMENTS_FILE" \
        "$nested"
      ;;
  esac
done < <(find "$REMOTE_APP_DIR" \( -name "*.framework" -o -name "*.dylib" -o -name "*.appex" \) -print0)

executable_name="$(plist_read "$REMOTE_APP_DIR/Info.plist" CFBundleExecutable)"
[[ -n "$executable_name" ]] ||
  die "missing CFBundleExecutable in Info.plist"

main_executable="$REMOTE_APP_DIR/$executable_name"
[[ -f "$main_executable" ]] ||
  die "missing main executable: $main_executable"

unlock_keychain_if_possible

log "signing main executable"
codesign --force \
  --sign "$identity" \
  --entitlements "$ENTITLEMENTS_FILE" \
  --verbose=4 \
  "$main_executable"

log "signing app bundle"
codesign --force \
  --sign "$identity" \
  --entitlements "$ENTITLEMENTS_FILE" \
  --verbose=4 \
  "$REMOTE_APP_DIR"

log "verifying signature"
codesign --verify --deep --strict --verbose=2 "$REMOTE_APP_DIR"

log "creating IPA"
rm -f "$REMOTE_IPA"
(
  cd "$REMOTE_ROOT"
  zip -qry "$REMOTE_IPA" Payload
)

[[ -f "$REMOTE_IPA" ]] ||
  die "failed to create IPA: $REMOTE_IPA"

log "signed IPA: $REMOTE_IPA"
REMOTE_SCRIPT_EOF
}

cmd_mac_codesign() {
  local tmp_script="/tmp/sign-on-mac.sh.$$"

  [[ -n "${MAC_HOST:-}" ]] ||
    mac_codesign_die "MAC_HOST is not set"

  [[ -d "$LOCAL_APP_DIR" ]] ||
    mac_codesign_die "missing app bundle: $LOCAL_APP_DIR"

  command -v rsync >/dev/null 2>&1 ||
    mac_codesign_die "missing rsync"

  command -v ssh >/dev/null 2>&1 ||
    mac_codesign_die "missing ssh"

  mac_codesign_log "remote mac: $MAC_HOST"
  mac_codesign_log "local app:  $LOCAL_APP_DIR"
  mac_codesign_log "bundle id:  $BUNDLE_ID"

  mac_codesign_log "preparing remote workspace"
  ssh "$MAC_HOST" "rm -rf '$REMOTE_ROOT' && mkdir -p '$REMOTE_PAYLOAD_DIR'"

  mac_codesign_log "copying app bundle to Mac"
  rsync -a --delete "$LOCAL_APP_DIR/" "$MAC_HOST:$REMOTE_APP_DIR/"

  local remote_entitlements=""
  local remote_provision=""

  if [[ -n "$ENTITLEMENTS_FILE" ]]; then
    [[ -f "$ENTITLEMENTS_FILE" ]] ||
      mac_codesign_die "missing local entitlements file: $ENTITLEMENTS_FILE"

    remote_entitlements="$REMOTE_ROOT/entitlements.plist"

    mac_codesign_log "copying local entitlements to Mac"
    rsync -a "$ENTITLEMENTS_FILE" "$MAC_HOST:$remote_entitlements"
  fi

  if [[ -n "$MOBILEPROVISION" && -f "$MOBILEPROVISION" ]]; then
    remote_provision="$REMOTE_ROOT/profile.mobileprovision"

    mac_codesign_log "copying local provisioning profile to Mac"
    rsync -a "$MOBILEPROVISION" "$MAC_HOST:$remote_provision"
  else
    # Treat MOBILEPROVISION as a remote Mac path.
    # Empty means the remote script will auto-find a matching profile.
    remote_provision="$MOBILEPROVISION"
  fi

  generate_remote_mac_codesign_script "$tmp_script"

  mac_codesign_log "copying signing script to Mac"
  rsync -a "$tmp_script" "$MAC_HOST:$REMOTE_SCRIPT"
  rm -f "$tmp_script"

  ssh "$MAC_HOST" "chmod +x '$REMOTE_SCRIPT'"

  mac_codesign_log "signing on Mac"
  ssh -tt "$MAC_HOST" \
    "'$REMOTE_SCRIPT' '$APP_NAME' '$BUNDLE_ID' '$REMOTE_ROOT' '$REMOTE_APP_DIR' '$REMOTE_IPA' '$SIGN_IDENTITY' '$remote_provision' '$remote_entitlements'"

  mac_codesign_log "copying signed IPA back"
  mkdir -p "$(dirname "$LOCAL_SIGNED_IPA")"
  rsync -a "$MAC_HOST:$REMOTE_IPA" "$LOCAL_SIGNED_IPA"

  [[ -f "$LOCAL_SIGNED_IPA" ]] ||
    mac_codesign_die "signed IPA was not copied back: $LOCAL_SIGNED_IPA"

  mac_codesign_log "signed IPA ready: $LOCAL_SIGNED_IPA"
}