/*
 * SPDX-License-Identifier: Apache-2.0
 * Based on Waveshare ESP32-S3-Touch-LCD-7B examples at c652c902.
 * Modified for NotchAgent Desk: Arduino Wire, USB routing, no battery/CAN/SD.
 */
#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace desk_board {

constexpr uint8_t kExpanderAddress = 0x24;
constexpr uint8_t kModeRegister = 0x02;
constexpr uint8_t kOutputRegister = 0x03;
constexpr uint8_t kPwmRegister = 0x05;
constexpr uint8_t kTouchReset = 1;
constexpr uint8_t kBacklight = 2;
constexpr uint8_t kLcdReset = 3;
constexpr uint8_t kUsbCanSelect = 5;
inline uint8_t outputState = 0xFF;

inline bool write(uint8_t command, uint8_t value) {
  Wire.beginTransmission(kExpanderAddress);
  Wire.write(command);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

inline bool setOutput(uint8_t pin, bool high) {
  if (high) outputState |= 1U << pin;
  else outputState &= ~(1U << pin);
  return write(kOutputRegister, outputState);
}

inline bool begin() {
  if (!Wire.begin(DESK_I2C_SDA, DESK_I2C_SCL, 400000)) return false;
  if (!write(kModeRegister, 0xFF)) return false;
  if (!setOutput(kUsbCanSelect, false)) return false;
  setOutput(kLcdReset, false);
  setOutput(kTouchReset, false);
  delay(20);
  setOutput(kLcdReset, true);
  pinMode(DESK_TOUCH_INTERRUPT, OUTPUT);
  digitalWrite(DESK_TOUCH_INTERRUPT, LOW);
  delay(100);
  setOutput(kTouchReset, true);
  delay(200);
  pinMode(DESK_TOUCH_INTERRUPT, INPUT_PULLUP);
  return setOutput(kBacklight, true);
}

inline bool setBacklight(uint8_t brightness) {
  return write(kPwmRegister, min<uint8_t>(brightness, 247));
}

}  // namespace desk_board
