#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
gate="${1:-all}"

contract() {
  "$root/tools/test-semver.sh"
  "$root/tools/check-release-contract.sh"
  git -C "$root" diff --check
}

apple() {
  "$root/tools/test-release-manifest.sh"
  swift test --package-path "$root/sdk/apple"
}

windows() {
  if ! command -v dotnet >/dev/null 2>&1; then
    local isolated="$HOME/Library/Application Support/NotchAgent/dotnet-8"
    [[ -x "$isolated/dotnet" ]] || { echo "dotnet 8 is required." >&2; exit 1; }
    export DOTNET_ROOT="$isolated"
    export PATH="$DOTNET_ROOT:$PATH"
  fi
  dotnet test "$root/sdk/dotnet/NotchAgent.Desk.Protocol.Tests/NotchAgent.Desk.Protocol.Tests.csproj" \
    --configuration Release
}

firmware() {
  export ARDUINO_DIRECTORIES_USER="${ARDUINO_DIRECTORIES_USER:-$HOME/Library/Application Support/NotchAgent/Arduino-RevA}"
  "$root/firmware/notchagent_desk/build.sh" build
}

case "$gate" in
  contract) contract ;;
  apple) apple ;;
  windows) windows ;;
  firmware) firmware ;;
  all) contract; apple; windows; firmware ;;
  *) echo "Usage: ./tools/preflight.sh [all|contract|apple|windows|firmware]" >&2; exit 2 ;;
esac
