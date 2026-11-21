---
name: delete-comment
description: Classify and remove code comments in NotchAgent Desk while preserving decisions in ADRs and enforcing the comment lint.
---

# Delete comments

1. Inventory tracked source and configuration files, then count actual comments without counting markers inside strings, URLs, shebangs, or the required Swift tools version directive.
2. Read each comment with its code. Delete tautologies and stale text. For implementation excuses, delete the comment and open a specific issue. Move non-obvious decisions to `docs/adr/` with context and consequences.
3. Run `python3 tools/check-comments.py`. Keep its per-file baseline at zero when possible; if a file cannot be cleaned in this change, record its current count without permitting increases in that file.
4. Run the release contract, Swift and .NET protocol tests, and firmware checks applicable to the changed files. Review the diff, then use the repository's single-PR delivery flow.
5. Report classifications, removed comment lines, remaining allowances, issues, ADRs, and any hardware proof still pending.
