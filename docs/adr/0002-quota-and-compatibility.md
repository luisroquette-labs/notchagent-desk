# Quota, mascot, and compatibility decisions

Status: accepted

Haiku, Sonnet, and Opus share one measured quota pool. The Desk protocol does not report a separate quota for each model, so their mascots use the same quota distress value. Per-model status remains visible through its existing tint. A missing provider reading (`-1`) stays distinct from a measured zero: the mascot freezes and dims instead of looking calmly measured. A limited status does not force an additional distress floor because that made distinct cards look equally exhausted.

The mascot uses the existing pixel glyph and maps the app's resting and distressed breathing rates to the same quota range. This keeps the Mac and Desk depictions consistent without creating a second visual scale.

An older app build may omit the current-hour fields. The Desk preserves `-1` as their unknown sentinel rather than displaying hour zero as current. Source colors, status meanings, and chart projection rules should be reviewed against the host app when the protocol changes.

Arduino's generated prototype pass does not reach functions inside the sketch's anonymous namespace. The ordered C++ sketch supplies the necessary forward declaration itself. Packaging consumes the ctags input without generating prototypes because the legacy pass has caused nondeterministic local and factory build hangs.

The ctags override is implementation debt tracked in issue #5. Replace it only after both normal and factory builds pass through a supported build path.
