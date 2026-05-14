#!/usr/bin/env bash
set -euo pipefail

generate_info_plist() {
  local out_plist="${1:-$IOS_APP_DIR/Info.plist}"
  local version_json="$PROJECT_ROOT/synthem/versioning/version.json"

  [[ -f "$version_json" ]] || die "missing version file: $version_json"

  local app_name channel major minor patch build version
  app_name="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["name"])' "$version_json")"
  channel="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("channel", "dev"))' "$version_json")"
  major="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("major", 0))' "$version_json")"
  minor="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("minor", 0))' "$version_json")"
  patch="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("patch", 0))' "$version_json")"
  build="$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1])).get("build", 0))' "$version_json")"
  version="${major}.${minor}.${patch}"

  run mkdir -p "$(dirname "$out_plist")"
  cat > "$out_plist" <<EOF
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
  <string>${app_name}</string>
  <key>CFBundleIdentifier</key>
  <string>${BUNDLE_ID}</string>
  <key>CFBundleName</key>
  <string>${app_name}</string>
  <key>CFBundleDisplayName</key>
  <string>${app_name}</string>
  <key>CFBundleShortVersionString</key>
  <string>${version}</string>
  <key>CFBundleVersion</key>
  <string>${build}</string>
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

  log "generated Info.plist: $out_plist"
  log "version: ${app_name} ${version} (${channel}, build ${build})"
}

cmd_plist() {
  local out="${1:-$IOS_APP_DIR/Info.plist}"
  generate_info_plist "$out"
}
