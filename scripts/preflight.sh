#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

"$root/tools/check-release-contract.sh"
swift test --package-path "$root/sdk/apple"
DOTNET_ROLL_FORWARD=Major dotnet test "$root/sdk/dotnet/NotchAgent.Desk.Protocol.Tests/NotchAgent.Desk.Protocol.Tests.csproj" --configuration Release
"$root/firmware/notchagent_desk/build.sh" build
