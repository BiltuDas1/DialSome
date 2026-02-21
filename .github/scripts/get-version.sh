#!/bin/bash
set -e

# Path to your manifest file
MANIFEST_FILE="android/AndroidManifest.xml"

# Extract versionName using grep and sed
VERSION=$(grep -oP 'android:versionName="\K[^"]+' "$MANIFEST_FILE")

# Fallback check if VERSION is empty
if [ -z "$VERSION" ]; then
  echo "Error: Could not find versionName in $MANIFEST_FILE"
  exit 1
fi

IS_PRERELEASE="false"

# Check for pre-release indicators
if [[ "$VERSION" =~ [ab] ]] || [[ "$VERSION" =~ "rc" ]] || [[ "$VERSION" == *"alpha"* ]] || [[ "$VERSION" == *"beta"* ]]; then
  IS_PRERELEASE="true"
fi

# Exporting to GITHUB_OUTPUT
echo "VERSION=$VERSION" >> $GITHUB_OUTPUT
echo "IS_PRERELEASE=$IS_PRERELEASE" >> $GITHUB_OUTPUT
