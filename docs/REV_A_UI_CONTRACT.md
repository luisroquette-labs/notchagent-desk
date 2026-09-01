# Rev A UI contract

Status: implemented in `firmware/notchagent_desk/notchagent_desk.ino`; physical review pending.

## Canvas and navigation

| Element | Contract |
|---|---|
| Canvas | 1024x600 RGB565 |
| Content | 1000x464, below the status bar |
| Navigation | NOW, BURN, RHYTHM, MODELS; touch targets at least 44 px |
| Gestures | Tap, horizontal swipe and scrub; no multitouch dependency |

## Information hierarchy

NOW is the glance screen: provider quota, reset/window, burn rate and model activity. BURN,
RHYTHM and MODELS expose detail without duplicating the primary metrics.

The UI must represent these states explicitly: no snapshot, connection lost, stale snapshot,
provider unavailable and reset unknown. It must never invent a reset timestamp or usage value.

## Acceptance

Visual approval requires a real Rev A unit. Record legibility, target accuracy, latency, FPS,
tearing and every state above in `REV_A_HARDWARE_QUALIFICATION.md` before promotion.
