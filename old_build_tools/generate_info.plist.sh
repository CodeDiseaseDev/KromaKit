#!/usr/bin/env bash
set -euo pipefail



SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/lib/paths.sh
source "$PROJECT_ROOT/scripts/lib/paths.sh"

VERSION_JSON="$SCRIPT_DIR/synthem/versioning/version.json"

DEFAULT_OUT_PLIST="$SCRIPT_DIR/$BUILD_IOS_PACKAGE_DIR/Payload/Synthem.app/Info.plist"
OUT_PLIST="${1:-$DEFAULT_OUT_PLIST}"

APP_NAME="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["name"])' "$VERSION_JSON")"
CHANNEL="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("channel", "dev"))' "$VERSION_JSON")"
MAJOR="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("major", 0))' "$VERSION_JSON")"
MINOR="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("minor", 0))' "$VERSION_JSON")"
PATCH="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("patch", 0))' "$VERSION_JSON")"
BUILD="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("build", 0))' "$VERSION_JSON")"

VERSION="${MAJOR}.${MINOR}.${PATCH}"

mkdir -p "$(dirname "$OUT_PLIST")"

cat > "$OUT_PLIST" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>UISupportedInterfaceOrientations</key>
  <array>
    <string>UIInterfaceOrientationPortrait</string>
    <string>UIInterfaceOrientationLandscapeLeft</string>
    <string>UIInterfaceOrientationLandscapeRight</string>
  </array>

  <key>UISupportedInterfaceOrientations~ipad</key>
  <array>
    <string>UIInterfaceOrientationPortrait</string>
    <string>UIInterfaceOrientationPortraitUpsideDown</string>
    <string>UIInterfaceOrientationLandscapeLeft</string>
    <string>UIInterfaceOrientationLandscapeRight</string>
  </array>

  <key>UIDeviceFamily</key>
  <array>
    <integer>1</integer>
    <integer>2</integer>
  </array>

  <key>CFBundleExecutable</key>
  <string>${APP_NAME}</string>

  <key>CFBundleIdentifier</key>
  <string>com.codelmao.synthem</string>

  <key>CFBundleName</key>
  <string>${APP_NAME}</string>

  <key>CFBundleDisplayName</key>
  <string>${APP_NAME}</string>

  <key>CFBundleShortVersionString</key>
  <string>${VERSION}</string>

  <key>CFBundleVersion</key>
  <string>${BUILD}</string>

  <key>UILaunchScreen</key>
  <dict/>

  <key>CADisableMinimumFrameDurationOnPhone</key>
  <true/>

  <key>CFBundleIcons</key>
  <dict>
      <key>CFBundlePrimaryIcon</key>
      <dict>
          <key>CFBundleIconFiles</key>
          <array>
              <string>icon</string>
          </array>
      </dict>
  </dict>

  <key>CFBundleIcons</key>
  <dict>
    <key>CFBundlePrimaryIcon</key>
    <dict>
      <key>CFBundleIconFiles</key>
      <array>
        <string>Icon-20</string>
        <string>Icon-29</string>
        <string>Icon-40</string>
        <string>Icon-60</string>
      </array>
      <key>UIPrerenderedIcon</key>
      <false/>
    </dict>
  </dict>

  <key>CFBundleIcons~ipad</key>
  <dict>
    <key>CFBundlePrimaryIcon</key>
    <dict>
      <key>CFBundleIconFiles</key>
      <array>
        <string>Icon-20</string>
        <string>Icon-29</string>
        <string>Icon-40</string>
        <string>Icon-76</string>
        <string>Icon-83.5</string>
      </array>
      <key>UIPrerenderedIcon</key>
      <false/>
    </dict>
  </dict>
</dict>
</plist>
EOF

echo "Generated $OUT_PLIST"
#echo "${APP_NAME} v${VERSION} build ${BUILD} ${CHANNEL}"
