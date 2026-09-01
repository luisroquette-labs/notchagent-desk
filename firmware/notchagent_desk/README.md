# NotchAgent Desk firmware

USB-only companion firmware being migrated to NotchAgent Desk Rev A: Waveshare
ESP32-S3-Touch-LCD-7B (1024x600, capacitive touch, 5-point). The checked-in
display adapter is still the Legacy Prototype Guition 480x320 / AXS15231B until
the Rev A migration lands. The NotchAgent host remains the only source of usage data and
credentials: macOS and Windows Desk paths remain beta until their physical
release gates pass. See [`../../COMPATIBILITY.md`](../../COMPATIBILITY.md).

## Security boundary

- No Wi-Fi, BLE, provider token, API request, filesystem, or remote update.
- A nonce handshake must complete before the Mac sends a snapshot.
- Frames use COBS resynchronization, CRC32, a 16 KiB limit, and protocol-major validation.
- Snapshots are held only in RAM and cleared after 15 minutes without an update,
  except while the Mac has intentionally paused refreshes.

## Build

Required versions: esp32 core 3.3.8, LVGL 9.2.2, Arduino_GFX 1.6.5, and
ArduinoJson 7.2.0, built with Arduino CLI 1.5.1. Builds fail closed when any
installed version drifts. The signed build-input fingerprint covers firmware
sources, the custom partition table, FQBN/flags, manifest generation, image
trimming, and the toolchain verifier.

```sh
./build.sh
./build.sh upload /dev/cu.usbmodemXXXX
```

The upload command deliberately has no default port.
