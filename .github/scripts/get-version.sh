#!/bin/bash
set -e

# Extract version from version.txt
# This reads the first line and removes any leading/trailing whitespace
VERSION=$(head -n 1 version.txt | xargs)

IS_PRERELEASE="false"

# Check for pre-release indicators
# Logic: Checks for 'a' (alpha), 'b' (beta), 'rc', or literal 'alpha'/'beta' strings
if [[ "$VERSION" =~ [ab] ]] || [[ "$VERSION" =~ "rc" ]] || [[ "$VERSION" == *"alpha"* ]] || [[ "$VERSION" == *"beta"* ]]; then
  IS_PRERELEASE="true"
fi

# Exporting to GITHUB_OUTPUT
echo "VERSION=$VERSION" >> $GITHUB_OUTPUT
echo "IS_PRERELEASE=$IS_PRERELEASE" >> $GITHUB_OUTPUT