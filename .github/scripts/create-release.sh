#!/bin/bash
set -e

gh release create "$VERSION" \
  --title "Release $VERSION" \
  --notes-file changelog.md \
  --prerelease=$IS_PRERELEASE