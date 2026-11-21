# Desk display and input decisions

Status: accepted

The desk display mirrors the macOS app's dark palette and model colors. Changes to those colors should be checked against the app's validated theme; independent tuning would make the same status appear different across devices.

The burn and hourly charts use shared geometry for drawing and touch hit testing. Both touch handlers select the nearest visible series or bar on each move, so dragging can change the selected item. The current hour is projected only after 15% of it has elapsed; earlier extrapolation is too noisy. The projected part is hollow to distinguish it from measured usage. Its geometry is a visual approximation because this LVGL configuration has no inexpensive dashed border.

Touch coordinates are read only after an interrupt. Periodic I2C traffic checks controller presence without inventing samples on an idle panel. The eight-byte read command came from the board's validated bring-up and needs hardware validation before changing.

Only consecutive animation frames contribute to the active frame-rate measurement. Sparse redraws on a static page are not evidence of poor rendering cadence. A paused host preserves its last valid snapshot rather than treating the pause as stale data.
