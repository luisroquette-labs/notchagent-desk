/*
 * SPDX-License-Identifier: Apache-2.0
 * GT911 register flow derived from Espressif/Waveshare examples at c652c902.
 * Modified for NotchAgent Desk: minimal single-pointer LVGL adapter.
 */
#pragma once

#include <Arduino.h>
#include <Wire.h>

class DeskTouch {
 public:
  bool begin() {
    instance_ = this;
    attachInterrupt(digitalPinToInterrupt(DESK_TOUCH_INTERRUPT), onInterrupt, FALLING);
    uint8_t product[3] = {};
    controllerPresent_ = readRegister(0x8140, product, sizeof(product));
    return controllerPresent_;
  }

  bool read(uint16_t &x, uint16_t &y) {
    const uint32_t now = millis();
    const bool interrupted = interruptPending_;
    if (!interrupted && now - lastPollAtMs_ < DESK_TOUCH_POLL_INTERVAL_MS) return false;
    interruptPending_ = false;
    if (!interrupted) {
      lastPollAtMs_ = now;
      ++pollAttemptCount_;
    }

    uint8_t status = 0;
    if (!readRegister(0x814E, &status, 1)) return readFailed();
    const uint8_t points = status & 0x0F;
    if (!(status & 0x80)) return false;
    if (points == 0 || points > 5) {
      if (!writeRegister(0x814E, 0)) return readFailed();
      return false;
    }

    uint8_t point[8] = {};
    if (!readRegister(0x814F, point, sizeof(point)) || !writeRegister(0x814E, 0)) {
      return readFailed();
    }
    x = min<uint16_t>(point[1] | (point[2] << 8), DESK_SCREEN_WIDTH - 1);
    y = min<uint16_t>(point[3] | (point[4] << 8), DESK_SCREEN_HEIGHT - 1);
    controllerPresent_ = true;
    ++touchCount_;
    if (!interrupted) ++pollTouchCount_;
    lastLatencyMicros_ = interrupted ? micros() - interruptAtMicros_ : 0;
    maxLatencyMicros_ = max(maxLatencyMicros_, lastLatencyMicros_);
    return true;
  }

  uint32_t touchCount() const { return touchCount_; }
  uint32_t interruptCount() const { return interruptCount_; }
  uint32_t readErrorCount() const { return readErrorCount_; }
  uint32_t pollAttemptCount() const { return pollAttemptCount_; }
  uint32_t pollTouchCount() const { return pollTouchCount_; }
  bool controllerPresent() const { return controllerPresent_; }
  uint32_t lastLatencyMicros() const { return lastLatencyMicros_; }
  uint32_t maxLatencyMicros() const { return maxLatencyMicros_; }

 private:
  inline static DeskTouch *instance_ = nullptr;
  volatile bool interruptPending_ = false;
  volatile uint32_t interruptAtMicros_ = 0;
  volatile uint32_t interruptCount_ = 0;
  uint32_t touchCount_ = 0;
  uint32_t readErrorCount_ = 0;
  uint32_t pollAttemptCount_ = 0;
  uint32_t pollTouchCount_ = 0;
  uint32_t lastPollAtMs_ = 0;
  bool controllerPresent_ = false;
  uint32_t lastLatencyMicros_ = 0;
  uint32_t maxLatencyMicros_ = 0;

  bool readRegister(uint16_t reg, uint8_t *bytes, size_t count) {
    Wire.beginTransmission(DESK_TOUCH_ADDRESS);
    Wire.write(static_cast<uint8_t>(reg >> 8));
    Wire.write(static_cast<uint8_t>(reg));
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(DESK_TOUCH_ADDRESS, count) != count) return false;
    for (size_t i = 0; i < count; ++i) bytes[i] = Wire.read();
    return true;
  }

  bool writeRegister(uint16_t reg, uint8_t value) {
    Wire.beginTransmission(DESK_TOUCH_ADDRESS);
    Wire.write(static_cast<uint8_t>(reg >> 8));
    Wire.write(static_cast<uint8_t>(reg));
    Wire.write(value);
    return Wire.endTransmission() == 0;
  }

  bool readFailed() {
    controllerPresent_ = false;
    ++readErrorCount_;
    return false;
  }

  static void ARDUINO_ISR_ATTR onInterrupt() {
    if (!instance_) return;
    instance_->interruptAtMicros_ = micros();
    instance_->interruptPending_ = true;
    ++instance_->interruptCount_;
  }
};
