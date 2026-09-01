/*
 * SPDX-License-Identifier: Apache-2.0
 * Based on Waveshare ESP32-S3-Touch-LCD-7B examples at c652c902.
 * Modified for NotchAgent Desk: minimal LVGL direct double-buffer adapter.
 */
#pragma once

#include <Arduino.h>
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"

namespace desk_display {

static esp_lcd_panel_handle_t panel = nullptr;
static TaskHandle_t flushTask = nullptr;

IRAM_ATTR static bool onVsync(esp_lcd_panel_handle_t,
                              const esp_lcd_rgb_panel_event_data_t *, void *) {
  BaseType_t awake = pdFALSE;
  if (flushTask) xTaskNotifyFromISR(flushTask, 1, eSetBits, &awake);
  return awake == pdTRUE;
}

inline bool begin(void **firstBuffer, void **secondBuffer) {
  const esp_lcd_rgb_panel_config_t config = {
    .clk_src = LCD_CLK_SRC_DEFAULT,
    .timings = {
      .pclk_hz = DESK_LCD_PIXEL_CLOCK_HZ,
      .h_res = DESK_SCREEN_WIDTH,
      .v_res = DESK_SCREEN_HEIGHT,
      .hsync_pulse_width = 162,
      .hsync_back_porch = 152,
      .hsync_front_porch = 48,
      .vsync_pulse_width = 45,
      .vsync_back_porch = 13,
      .vsync_front_porch = 3,
      .flags = {.pclk_active_neg = 1},
    },
    .data_width = 16,
    .bits_per_pixel = 16,
    .num_fbs = 2,
    .bounce_buffer_size_px = DESK_SCREEN_WIDTH * 10,
    .sram_trans_align = 4,
    .psram_trans_align = 64,
    .hsync_gpio_num = GPIO_NUM_46,
    .vsync_gpio_num = GPIO_NUM_3,
    .de_gpio_num = GPIO_NUM_5,
    .pclk_gpio_num = GPIO_NUM_7,
    .disp_gpio_num = GPIO_NUM_NC,
    .data_gpio_nums = {GPIO_NUM_14, GPIO_NUM_38, GPIO_NUM_18, GPIO_NUM_17,
                       GPIO_NUM_10, GPIO_NUM_39, GPIO_NUM_0, GPIO_NUM_45,
                       GPIO_NUM_48, GPIO_NUM_47, GPIO_NUM_21, GPIO_NUM_1,
                       GPIO_NUM_2, GPIO_NUM_42, GPIO_NUM_41, GPIO_NUM_40},
    .flags = {.fb_in_psram = 1},
  };
  if (esp_lcd_new_rgb_panel(&config, &panel) != ESP_OK) return false;
  if (esp_lcd_panel_init(panel) != ESP_OK) return false;
  const esp_lcd_rgb_panel_event_callbacks_t callbacks = {
    .on_frame_buf_complete = onVsync,
  };
  if (esp_lcd_rgb_panel_register_event_callbacks(panel, &callbacks, nullptr) != ESP_OK) return false;
  return esp_lcd_rgb_panel_get_frame_buffer(panel, 2, firstBuffer, secondBuffer) == ESP_OK;
}

inline void flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels) {
  if (lv_display_flush_is_last(display)) {
    flushTask = xTaskGetCurrentTaskHandle();
    ulTaskNotifyValueClear(nullptr, UINT32_MAX);
    if (esp_lcd_panel_draw_bitmap(panel, area->x1, area->y1, area->x2 + 1,
                                  area->y2 + 1, pixels) == ESP_OK) {
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
    }
    flushTask = nullptr;
  }
  lv_display_flush_ready(display);
}

}  // namespace desk_display
