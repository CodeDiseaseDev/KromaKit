#!/usr/bin/env bash
set -euo pipefail

generate_info_plist() {
  local out_plist="${1:-$IOS_APP_DIR/Info.plist}"
  local out_storyboard="${2:-$IOS_APP_DIR/LaunchScreen.storyboard}"
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
  <key>UILaunchStoryboardName</key>
  <string>LaunchScreen</string>
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
  <key>MinimumOSVersion</key>
  <string>13.0</string>
  <key>CFBundlePackageType</key>
  <string>APPL</string>

  <key>CFBundleSupportedPlatforms</key>
  <array>
    <string>iPhoneOS</string>
  </array>
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

#  cat > "$out_storyboard" <<EOF
#<?xml version="1.0" encoding="UTF-8"?>
#<document type="com.apple.InterfaceBuilder3.CocoaTouch.Storyboard.XIB" version="3.0" toolsVersion="22505" targetRuntime="iOS.CocoaTouch" propertyAccessControl="none" useAutolayout="YES" launchScreen="YES" useTraitCollections="YES" colorMatched="YES" initialViewController="launch">
#    <device id="retina6_12" orientation="portrait" appearance="dark"/>
#    <dependencies>
#        <deployment identifier="iOS"/>
#        <plugIn identifier="com.apple.InterfaceBuilder.IBCocoaTouchPlugin" version="22504"/>
#        <capability name="Safe area layout guides" minToolsVersion="9.0"/>
#        <capability name="documents saved in the Xcode 8 format" minToolsVersion="8.0"/>
#    </dependencies>
#    <scenes>
#        <scene sceneID="launch-scene">
#            <objects>
#                <viewController id="launch" sceneMemberID="viewController">
#                    <view key="view" contentMode="scaleToFill" id="launch-view">
#                        <rect key="frame" x="0.0" y="0.0" width="393" height="852"/>
#                        <autoresizingMask key="autoresizingMask" widthSizable="YES" heightSizable="YES"/>
#                        <subviews>
#                            <label opaque="NO" userInteractionEnabled="NO" contentMode="left" horizontalHuggingPriority="251" verticalHuggingPriority="251" text="Synthem" textAlignment="center" lineBreakMode="tailTruncation" baselineAdjustment="alignBaselines" adjustsFontSizeToFit="NO" translatesAutoresizingMaskIntoConstraints="NO" id="title-label">
#                                <rect key="frame" x="96" y="398" width="201" height="56"/>
#                                <fontDescription key="fontDescription" type="boldSystem" pointSize="46"/>
#                                <color key="textColor" red="0.94" green="0.90" blue="1" alpha="1" colorSpace="custom" customColorSpace="sRGB"/>
#                                <nil key="highlightedColor"/>
#                            </label>
#                        </subviews>
#                        <viewLayoutGuide key="safeArea" id="safe-area"/>
#                        <color key="backgroundColor" red="0.05" green="0.03" blue="0.08" alpha="1" colorSpace="custom" customColorSpace="sRGB"/>
#                        <constraints>
#                            <constraint firstItem="title-label" firstAttribute="centerX" secondItem="launch-view" secondAttribute="centerX" id="title-center-x"/>
#                            <constraint firstItem="title-label" firstAttribute="centerY" secondItem="launch-view" secondAttribute="centerY" id="title-center-y"/>
#                        </constraints>
#                    </view>
#                </viewController>
#                <placeholder placeholderIdentifier="IBFirstResponder" id="first-responder" userLabel="First Responder" sceneMemberID="firstResponder"/>
#            </objects>
#            <point key="canvasLocation" x="0.0" y="0.0"/>
#        </scene>
#    </scenes>
#</document>
#EOF

  log "generated Info.plist: $out_plist"
#  log "generated LaunchScreen.storyboard: $out_storyboard"
  log "version: ${app_name} ${version} (${channel}, build ${build})"
}

cmd_plist() {
  local out="${1:-$IOS_APP_DIR/Info.plist}"
  local out_storyboard="${2:-$IOS_APP_DIR/LaunchScreen.storyboard}"
  generate_info_plist "$out" "$out_storyboard"
}
