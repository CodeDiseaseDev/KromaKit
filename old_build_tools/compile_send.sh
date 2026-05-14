#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"

iphone='mobile@192.168.0.133'

app_name="Synthem"
app_path="/var/mobile/Documents/${app_name}.ipa"
troll_store="uiopen 'apple-magnifier://install?url=file://${app_path}'"
unlock_wait_async="/var/jb/var/mobile/wait_for_unlock.sh"
sock="/tmp/ssh_iphone_socket"
bundle_identifier="com.codelmao.synthem"

# --- colors ---
RED="\033[31m"
GRN="\033[32m"
YLW="\033[33m"
BLU="\033[34m"
RST="\033[0m"

# --- helpers ---
log()   { echo -e "${BLU}[$app_name]${RST} $1"; }
ok()    { echo -e "${GRN}✔ $1${RST}"; }
fail()  { echo -e "${RED}✖ $1${RST}"; exit 1; }
warn()  { echo -e "${YLW}! $1${RST}"; }

start=$(date +%s)

wait_for_install() {
    while true; do
        if ssh -o ControlPath=$sock $iphone "uicache -l | grep -q com.codelmao.synthem"; then
            # wait a tiny bit and confirm again
#            sleep 0
            if ssh -o ControlPath=$sock $iphone "uicache -l | grep -q com.codelmao.synthem"; then
                return 0
            fi
        fi
#        sleep 0.1
    done
}

ssh -o ControlMaster=yes \
    -o ControlPath=$sock \
    -o ControlPersist=60 \
    -N $iphone 2>/dev/null &

clear

echo
log "Deploying to ${iphone}"
echo

## --- compile ---
#log "Compiling..."
#./compile_package.sh || fail "Compile failed"
#ok "Compiled"

# --- upload ---
log "Uploading .ipa..."
scp -o ControlPath=$sock "./$BUILD_IOS_PACKAGE_DIR/$app_name.ipa" $iphone:$app_path > /dev/null || fail "Upload failed"
ok "Uploaded"

log "Checking if iPhone is unlocked..."
ssh -o ControlPath=$sock $iphone "sh $unlock_wait_async" > /dev/null || warn "$unlock_wait_async failed."
ok "Unlocked"

# --- install ---
log "Installing..."
ssh -o ControlPath=$sock $iphone "$troll_store" > /dev/null || fail "Install failed"
wait_for_install
ok "Installed"

log "Waiting a sec... (we don't like race conditions)"
sleep 2
ok "Waited"

# --- launch ---
log "Launching..."
ssh -o ControlPath=$sock $iphone "open $bundle_identifier" > /dev/null || fail "Launch failed"
ok "App launched"

echo

end=$(date +%s)
ok "Done in $((end - start))s"
echo

log "Displaying logs..."
ssh -o ControlPath="$sock" "$iphone" "
    tail -n +1 -f /tmp/synthem.log &
    tpid=\$!

    while ps aux | grep Synthem | grep -v grep >/dev/null; do
        sleep 0.2
    done

    kill \$tpid 2>/dev/null
"

echo
ok "App closed."


# --- cleanup ---
ssh -o ControlPath=$sock -O exit $iphone 2>/dev/null
rm -f $sock
