# Rev A enclosure gate

Status: engineering constraints fixed; CAD, fabrication and physical fit are pending.

## Canonical mechanical input

Use the official Waveshare `hardware/dimensions/ESP32-S3-Touch-LCD-7B.stp` from commit
`c652c902db607f7ffb376257393cfd7657aa6428`. Expected SHA-256:
`c07b5bfc9f32903c89cfc7bf08baddd0c2040a2ad9e1fa281fc700cba2e05317`.

The design envelope must preserve the 192.96x110.76 mm glass area and approximately 18.3 mm
maximum board depth. Recheck all dimensions against the STEP before manufacturing.

## Non-negotiable constraints

| Area | Requirement |
|---|---|
| Glass and flats | no point load, pinch, sharp edge or assembly preload |
| Service | both USB-C ports, BOOT and RESET remain reachable and labelled |
| Radio and heat | antenna keep-out and rear ventilation remain unobstructed |
| Desk use | stable base, usable viewing angle, controlled reflections and no touch wobble |
| Scope | no battery cavity or battery validation in this revision |

CAD may be frozen only after a printed fit-check passes port alignment, flat clearance, thermal
soak, repeated touch and assembly/disassembly without damage.
