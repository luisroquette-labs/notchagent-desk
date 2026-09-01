#!/usr/bin/env bash
set -euo pipefail

sketch_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$sketch_dir/../../tools/semver.sh"
release_dir="${1:-$sketch_dir/release}"
manifest="$release_dir/manifest.json"
[[ -f "$manifest" ]] || { echo "Invalid firmware package: manifest missing." >&2; exit 1; }

jq -e '
  .schemaVersion == 3 and .chip == "esp32s3" and .imageAddress == 0 and
  .hardwareModel == "waveshare-esp32-s3-touch-lcd-7b" and
  .imageFile == "NotchAgentDesk-factory.bin" and .flasherFile == "esptool" and
  (.firmwareVersion | type == "string") and
  (.imageSHA256 | test("^[0-9a-f]{64}$")) and
  (.sourceSHA256 | test("^[0-9a-f]{64}$")) and
  (.flasherSHA256 | test("^[0-9a-f]{64}$"))
' "$manifest" >/dev/null || { echo "Invalid firmware manifest." >&2; exit 1; }

firmware_version="$(jq -er '.firmwareVersion' "$manifest")"
is_desk_semver "$firmware_version" || { echo "Invalid firmware SemVer." >&2; exit 1; }

image="$release_dir/NotchAgentDesk-factory.bin"
flasher="$release_dir/esptool"
[[ -f "$image" && -x "$flasher" ]] || { echo "Invalid firmware package: payload missing." >&2; exit 1; }

expected_image=$(jq -r '.imageSHA256' "$manifest")
expected_flasher=$(jq -r '.flasherSHA256' "$manifest")
actual_image=$(shasum -a 256 "$image" | awk '{print $1}')
actual_flasher=$(shasum -a 256 "$flasher" | awk '{print $1}')
[[ "$actual_image" == "$expected_image" && "$actual_flasher" == "$expected_flasher" ]] || {
  echo "Firmware package integrity check failed." >&2
  exit 1
}

echo "PASS: firmware package manifest and SHA-256 payloads verified."
