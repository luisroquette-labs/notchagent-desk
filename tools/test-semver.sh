#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "$root/tools/semver.sh"

for version in 0.8.0 1.0.0-alpha.1 1.0.0-beta.2 1.0.0-rc.0; do
  is_desk_semver "$version" || { echo "Rejected valid SemVer: $version" >&2; exit 1; }
done
for version in 01.0.0 1.0 1.0.0-alpha 1.0.0-preview.1 1.0.0-alpha.01; do
  if is_desk_semver "$version"; then
    echo "Accepted invalid or unsupported SemVer: $version" >&2
    exit 1
  fi
done

echo "PASS: Desk stable and prerelease SemVer validation"
