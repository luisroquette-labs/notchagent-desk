# Rev A hardware qualification

Status: **not run**. Do not promote or publish `1.0.0-alpha.1` until every required row has
evidence from the received ESP32-S3-Touch-LCD-7B.

## Unit record

| Field | Evidence |
|---|---|
| Board label and SKU | pending |
| Unit identifier | pending |
| Photos of glass, PCB, flats and connectors | pending |
| USB-C data cable identifier | pending |
| Official example commit | `c652c902db607f7ffb376257393cfd7657aa6428` |
| Rev A firmware commit and package hashes | pending |

Keep photos outside Git when they contain serial numbers. Store only sanitized paths and hashes
here. Battery, Wi-Fi, BLE, microSD data, CAN and RS485 are outside this qualification.

## Required results

| Gate | Required result | Evidence |
|---|---|---|
| Identity and inspection | exact 7B board; glass, flats, PCB and connectors undamaged | pending |
| Display | 1024x600 RGB565; correct colors; no dead pixels, corruption or irregular backlight | pending |
| Touch | all regions responsive; no ghost touches; official five-point example passes | pending |
| USB | native USB stable directly and through one known dock and hub | pending |
| UI | readable at desk distance; all states and targets in `REV_A_UI_CONTRACT.md` pass | pending |
| Stress | 100 reset/reconnect cycles and abrupt disconnect pass | pending |
| Runtime | record FPS, input latency, internal heap, PSRAM, temperature and tearing | pending |
| Soak | 24 hours; zero invalid frames, watchdogs, brownouts, ghost touches or corruption | pending |

## Rejection rule

Any failed required row blocks release. Record the observed result, firmware hash, power path and
reproduction steps; never replace a failure with an unchecked row.
