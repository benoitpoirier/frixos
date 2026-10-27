#ifndef F_SCREENSHOT_H
#define F_SCREENSHOT_H

#include "esp_err.h"
#include "esp_http_server.h"
#include "lvgl.h"

/*
 * Framebuffer-less screen capture.
 *
 * The device renders through a single 128x16 LVGL draw buffer, so a full
 * 128x128 image never exists in RAM. Instead, a capture invalidates the whole
 * screen and streams each strip to the HTTP client from LV_EVENT_FLUSH_START,
 * right before it is sent to the panel. Extra RAM: a few dozen bytes.
 *
 * Wire format of GET /api/screenshot (little-endian):
 *   "FXS1" | u16 width | u16 height
 *   then records: u16 x1, y1, x2, y2 (inclusive) + RGB565 pixels, row-major.
 */

void screenshot_attach(lv_display_t *disp);
esp_err_t screenshot_http_handler(httpd_req_t *req);

#endif // F_SCREENSHOT_H
