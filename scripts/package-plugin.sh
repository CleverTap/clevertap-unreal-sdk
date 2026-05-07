#!/usr/bin/env bash
# Packages the CleverTap Unreal plugin into a distributable zip.
# Usage: ./scripts/package-plugin.sh [version]
# If version is omitted, it is read from CleverTap.uplugin.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
PLUGIN_DIR="$REPO_ROOT/Plugins/CleverTap"
UPLUGIN="$PLUGIN_DIR/CleverTap.uplugin"

# Resolve version
if [[ $# -ge 1 ]]; then
    VERSION="$1"
else
    VERSION=$(python3 -c "import json,sys; d=json.load(open('$UPLUGIN')); print(d['VersionName'])")
fi

OUTPUT_ZIP="$REPO_ROOT/CleverTap-UnrealPlugin-v${VERSION}.zip"

echo "Packaging CleverTap Unreal Plugin v${VERSION}..."
echo "Source : $PLUGIN_DIR"
echo "Output : $OUTPUT_ZIP"

# Clean previous output
rm -f "$OUTPUT_ZIP"

cd "$REPO_ROOT/Plugins"

zip -r "$OUTPUT_ZIP" CleverTap \
    --exclude "CleverTap/Binaries/*" \
    --exclude "CleverTap/Intermediate/*" \
    --exclude "CleverTap/*.dylib" \
    --exclude "*/.dSYM/*" \
    --exclude "*/.DS_Store" \
    --exclude "*/__pycache__/*" \
    --exclude "*/CTNotificationService/include/*"

SIZE=$(du -sh "$OUTPUT_ZIP" | cut -f1)
echo ""
echo "Done. Package: $OUTPUT_ZIP ($SIZE)"
echo ""
echo "Clients: drop the 'CleverTap' folder from the zip into their project's Plugins/ directory."
