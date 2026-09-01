# Contributing

1. Open an issue describing the user-visible or protocol impact.
2. Use a branch named `feat/...`, `fix/...`, `docs/...`, or `chore/...`.
3. Use Conventional Commits and update `CHANGELOG.md` for notable changes.
4. Run `./tools/preflight.sh all`; missing tools or failed gates block the PR.
5. Never commit credentials, transcripts, device serials, customer data, or
   unsanitized physical-test reports.

Protocol and compatibility changes require an explicit versioning review under
[`VERSIONING.md`](VERSIONING.md).

GitHub `main` currently has no branch protection. Until protection is enabled,
direct pushes and merges without green checks on the latest PR SHA are
operationally prohibited.
