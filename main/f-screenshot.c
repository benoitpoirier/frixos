/*
 * f-screenshot.c — stream the LVGL display to an HTTP client without a
 * framebuffer. See f-screenshot.h for the design and wire format.
 */
#include "f-screenshot.h"
#include "frixos.h"
#include "esp_lvgl_port.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "screenshot";

#define SHOT_MIN_FREE_HEAP (10 * 1024) // refuse when the heap is tight (TLS handshakes)
#define SHOT_LOCK_MS 500               // wait for the LVGL lock to invalidate the screen
#define SHOT_FLUSH_WAIT_MS 500         // max stall of the LVGL task per strip
#define SHOT_STRIP_WAIT_MS 250         // HTTP side poll period
#define SHOT_TOTAL_MS 4000             // overall capture deadline

static lv_display_t *s_disp = NULL;
static StaticSemaphore_t s_ready_buf, s_done_buf;
static SemaphoreHandle_t s_ready = NULL; // LVGL -> HTTP: a strip is available
static SemaphoreHandle_t s_done = NULL;  // HTTP -> LVGL: strip sent, buffer can be released
static volatile bool s_active = false;
static lv_area_t s_area;
static const uint8_t *s_px = NULL;
static uint32_t s_stride = 0;

// Runs in the LVGL task, right before the strip is byte-swapped and sent to the panel.
static void flush_start_cb(lv_event_t *e)
{
  if (!s_active)
    return;
  const lv_area_t *a = (const lv_area_t *)lv_event_get_param(e);
  lv_draw_buf_t *buf = lv_display_get_buf_active(s_disp);
  if (!a || !buf || !buf->data)
    return;

  s_area = *a;
  s_px = buf->data;
  s_stride = buf->header.stride;
  xSemaphoreGive(s_ready);
  if (xSemaphoreTake(s_done, pdMS_TO_TICKS(SHOT_FLUSH_WAIT_MS)) != pdTRUE)
  {
    // Client too slow: abort the capture rather than freezing the display.
    s_active = false;
  }
}

void screenshot_attach(lv_display_t *disp)
{
  if (!disp || s_disp)
    return;
  s_ready = xSemaphoreCreateBinaryStatic(&s_ready_buf);
  s_done = xSemaphoreCreateBinaryStatic(&s_done_buf);
  s_disp = disp;
  lv_display_add_event_cb(disp, flush_start_cb, LV_EVENT_FLUSH_START, NULL);
}

static inline void put_u16(uint8_t *p, uint16_t v)
{
  p[0] = (uint8_t)(v & 0xFF);
  p[1] = (uint8_t)(v >> 8);
}

static esp_err_t send_strip(httpd_req_t *req)
{
  int32_t w = lv_area_get_width(&s_area);
  int32_t h = lv_area_get_height(&s_area);
  if (w <= 0 || h <= 0 || s_area.x1 < 0 || s_area.y1 < 0)
    return ESP_OK;

  uint8_t hdr[8];
  put_u16(hdr + 0, (uint16_t)s_area.x1);
  put_u16(hdr + 2, (uint16_t)s_area.y1);
  put_u16(hdr + 4, (uint16_t)s_area.x2);
  put_u16(hdr + 6, (uint16_t)s_area.y2);
  esp_err_t err = httpd_resp_send_chunk(req, (const char *)hdr, sizeof(hdr));

  // RGB565 is stored little-endian in the draw buffer (swap happens later, in the port flush).
  size_t row_bytes = (size_t)w * sizeof(uint16_t);
  if (err == ESP_OK && s_stride == row_bytes)
    return httpd_resp_send_chunk(req, (const char *)s_px, row_bytes * h);
  for (int32_t y = 0; err == ESP_OK && y < h; y++)
    err = httpd_resp_send_chunk(req, (const char *)(s_px + (size_t)y * s_stride), row_bytes);
  return err;
}

static esp_err_t send_busy(httpd_req_t *req, const char *why)
{
  ESP_LOG_WEB(ESP_LOG_WARN, TAG, "Capture refused: %s", why);
  httpd_resp_set_status(req, "503 Service Unavailable");
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Retry-After", "5");
  httpd_resp_sendstr(req, "{\"status\":\"error\",\"message\":\"busy\"}");
  return ESP_OK;
}

esp_err_t screenshot_http_handler(httpd_req_t *req)
{
  if (!s_disp)
    return send_busy(req, "display not ready");
  if (s_active)
    return send_busy(req, "capture in progress");
  if (heap_caps_get_free_size(MALLOC_CAP_8BIT) < SHOT_MIN_FREE_HEAP)
    return send_busy(req, "low memory");

  // Drop stale tokens left by a previous aborted capture.
  xSemaphoreTake(s_ready, 0);
  xSemaphoreTake(s_done, 0);

  if (!lvgl_port_lock(SHOT_LOCK_MS))
    return send_busy(req, "LVGL lock timeout");
  s_active = true;
  lv_obj_invalidate(lv_screen_active());
  int32_t W = lv_display_get_horizontal_resolution(s_disp);
  int32_t H = lv_display_get_vertical_resolution(s_disp);
  lvgl_port_unlock();

  httpd_resp_set_type(req, "application/octet-stream");
  httpd_resp_set_hdr(req, "Cache-Control", "no-store");

  uint8_t hdr[8] = {'F', 'X', 'S', '1'};
  put_u16(hdr + 4, (uint16_t)W);
  put_u16(hdr + 6, (uint16_t)H);
  esp_err_t err = httpd_resp_send_chunk(req, (const char *)hdr, sizeof(hdr));

  uint32_t remaining = (uint32_t)(W * H);
  int64_t deadline = esp_timer_get_time() + (int64_t)SHOT_TOTAL_MS * 1000;
  while (err == ESP_OK && remaining > 0 && s_active && esp_timer_get_time() < deadline)
  {
    if (xSemaphoreTake(s_ready, pdMS_TO_TICKS(SHOT_STRIP_WAIT_MS)) != pdTRUE)
      continue;
    err = send_strip(req);
    uint32_t n = lv_area_get_size(&s_area);
    remaining = (n >= remaining) ? 0 : remaining - n;
    xSemaphoreGive(s_done);
  }

  s_active = false;
  // Release a flush that may have slipped in between the last check and s_active = false.
  xSemaphoreGive(s_done);

  if (err != ESP_OK)
  {
    ESP_LOG_WEB(ESP_LOG_WARN, TAG, "Capture aborted: %s", esp_err_to_name(err));
    return ESP_FAIL;
  }
  if (remaining > 0)
    ESP_LOG_WEB(ESP_LOG_WARN, TAG, "Capture incomplete (%lu px missing)", (unsigned long)remaining);
  return httpd_resp_send_chunk(req, NULL, 0);
}
