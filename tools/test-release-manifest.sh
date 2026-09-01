#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
fixture="$(mktemp -d)"
cleanup() {
  [[ "$fixture" == "${TMPDIR:-/tmp}"* && -d "$fixture" ]] && rm -r -- "$fixture"
}
trap cleanup EXIT

printf 'firmware' > "$fixture/NotchAgentDesk-factory.bin"
printf '#!/bin/sh\nexit 0\n' > "$fixture/esptool"
chmod +x "$fixture/esptool"
source_sha="$(printf source | shasum -a 256 | awk '{print $1}')"
swift "$root/firmware/notchagent_desk/package_manifest.swift" 1.0.0-alpha.1 \
  "$fixture/NotchAgentDesk-factory.bin" "$fixture/esptool" "$source_sha" \
  "$fixture/manifest.json"
"$root/firmware/notchagent_desk/verify-release.sh" "$fixture" >/dev/null

jq '.hardwareModel = "unknown"' "$fixture/manifest.json" > "$fixture/invalid.json"
mv "$fixture/invalid.json" "$fixture/manifest.json"
if "$root/firmware/notchagent_desk/verify-release.sh" "$fixture" >/dev/null 2>&1; then
  echo "Accepted manifest for unknown hardware" >&2
  exit 1
fi

echo "PASS: schema 3 manifest binds firmware to Rev A hardware"
