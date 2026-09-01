# Versioning policy

NotchAgent Desk uses [Semantic Versioning 2.0.0](https://semver.org/) for every
published product release: `MAJOR.MINOR.PATCH[-PRERELEASE]`.

## Release meaning

- **MAJOR:** incompatible USB protocol, hardware revision, persisted-data, or
  host integration change requiring coordinated migration.
- **MINOR:** backward-compatible screen, telemetry, interaction, host SDK, or
  device capability.
- **PATCH:** backward-compatible correctness, reliability, security, copy, or
  packaging fix.
- **Prerelease:** `-alpha.N`, `-beta.N`, or `-rc.N`; never marketed as stable.

The repository `VERSION` is the Desk product and firmware version. Git tags use
`vX.Y.Z[-PRERELEASE]`. Released tags and artifacts are immutable.

## Hardware names

- **Rev A:** Waveshare ESP32-S3-Touch-LCD-7B, 1024x600 capacitive touch.
- **Rev B:** future coexisting hardware revision; not defined in this cycle.
- **Legacy Prototype:** Guition ESP32-S3 480x320 with AXS15231B.

Firmware `0.8.0` is the immutable recovery release for the Legacy Prototype.
Rev A starts at `1.0.0-alpha.1` because the hardware change is a MAJOR change.

## Wire protocol

The USB contract has an independent `MAJOR.MINOR` version in
`PROTOCOL_VERSION`:

- protocol **MAJOR** mismatch: connection is rejected;
- same MAJOR, newer protocol **MINOR**: additive fields and frame types must be
  ignored safely by older compatible hosts;
- removing or changing an existing field requires a protocol MAJOR bump and a
  Desk product MAJOR bump.

## Compatibility contract

Every release must update `COMPATIBILITY.md` with the minimum compatible:

1. NotchAgent host version;
2. firmware version;
3. protocol version;
4. macOS and Windows support state;
5. hardware revision.

No platform can move from `beta` to `stable` without its physical matrix gate.

## Commit and changelog rules

Commits follow [Conventional Commits 1.0.0](https://www.conventionalcommits.org/en/v1.0.0/):

```text
feat(firmware): add reconnect recovery state
fix(protocol): reject oversized telemetry payload
docs(compatibility): record Windows pilot matrix
```

`feat` implies MINOR, `fix` implies PATCH, and `BREAKING CHANGE:` implies
MAJOR. `CHANGELOG.md` follows Keep a Changelog categories: Added, Changed,
Deprecated, Removed, Fixed, and Security.

## Required release gates

1. `tools/check-release-contract.sh` passes.
2. Firmware compiles with the pinned toolchain.
3. Apple and .NET protocol SDK tests pass.
4. Current-platform physical compatibility evidence passes.
5. Tag, changelog, artifact manifest, and checksums agree.
