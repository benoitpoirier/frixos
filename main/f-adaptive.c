#include "f-adaptive.h"

#include "f-pwm.h"
#include "frixos.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "esp_attr.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"

#define ADAPT_MAX_STEP_PCT 3U
#define ADAPT_RTC_MAGIC 0x46525848U
#define ADAPT_RTC_RESET_REASON_POWER_ON ESP_RST_POWERON
#define ADAPT_RTC_RESET_REASON_BROWNOUT ESP_RST_BROWNOUT
#define ADAPT_SENSOR_MIN_LUX 0.1f
#define ADAPT_CRC_POLYNOMIAL 0xEDB88320U

static const char *TAG = "f-adaptive";

typedef struct
{
    uint32_t magic;
    int32_t day_key;
    uint16_t today[LUX_HIST_HOURS];
    uint16_t yesterday[LUX_HIST_HOURS];
    uint32_t crc;
} lux_history_t;

adaptive_curve_t eeprom_adapt_curve = {0};

RTC_NOINIT_ATTR static lux_history_t s_lux_history;

static portMUX_TYPE s_adaptive_mux = portMUX_INITIALIZER_UNLOCKED;
static int16_t s_curve_log_lux[ADAPT_CURVE_POINTS];
static int16_t s_current_log_lux;
static uint32_t s_hour_sum_x10;
static uint16_t s_hour_count;
static int8_t s_hour = -1;
static uint8_t s_applied_brightness;
static uint8_t s_state;

#define ADAPT_STATE_INITIALIZED 0x01U
#define ADAPT_STATE_HAS_LUX 0x02U
#define ADAPT_STATE_HAS_APPLIED 0x04U

static uint32_t history_crc32(const lux_history_t *history)
{
    const uint8_t *bytes = (const uint8_t *)history;
    uint32_t crc = UINT32_MAX;

    for (size_t i = 0; i < offsetof(lux_history_t, crc); i++)
    {
        crc ^= bytes[i];
        for (size_t bit = 0; bit < 8; bit++)
            crc = (crc >> 1) ^ ((crc & 1U) ? ADAPT_CRC_POLYNOMIAL : 0U);
    }

    return ~crc;
}

static bool history_valid(void)
{
    return s_lux_history.magic == ADAPT_RTC_MAGIC &&
           s_lux_history.crc == history_crc32(&s_lux_history);
}

static void history_reset(void)
{
    memset(&s_lux_history, 0, sizeof(s_lux_history));
    s_lux_history.magic = ADAPT_RTC_MAGIC;
    s_lux_history.day_key = LUX_HIST_INVALID_DAY;
    lux_history_clear(s_lux_history.today, s_lux_history.yesterday);
    s_lux_history.crc = history_crc32(&s_lux_history);
}

static int32_t local_day_key(const struct tm *local_time)
{
    int year = local_time->tm_year + 1900;
    const int month = local_time->tm_mon + 1;
    const unsigned day = (unsigned)local_time->tm_mday;
    year -= month <= 2;

    const int era = (year >= 0 ? year : year - 399) / 400;
    const unsigned year_of_era = (unsigned)(year - era * 400);
    const unsigned adjusted_month = (unsigned)(month + (month > 2 ? -3 : 9));
    const unsigned day_of_year = (153U * adjusted_month + 2U) / 5U + day - 1U;
    const unsigned day_of_era = year_of_era * 365U + year_of_era / 4U -
                                year_of_era / 100U + day_of_year;
    return (int32_t)(era * 146097 + (int32_t)day_of_era);
}

static void history_finish_hour_locked(void)
{
    if (s_hour >= 0 && s_hour < LUX_HIST_HOURS && s_hour_count > 0)
    {
        s_lux_history.today[(size_t)s_hour] =
            lux_history_average(s_hour_sum_x10, s_hour_count);
        s_lux_history.crc = history_crc32(&s_lux_history);
    }
}

static void history_sample(float lux)
{
    if (!time_valid || !isfinite(lux))
        return;

    time_t now;
    struct tm local_time;
    time(&now);
    if (localtime_r(&now, &local_time) == NULL ||
        local_time.tm_hour < 0 || local_time.tm_hour >= LUX_HIST_HOURS)
        return;

    const int32_t day_key = local_day_key(&local_time);
    const int8_t hour = (int8_t)local_time.tm_hour;
    const uint16_t sample_x10 = lux_history_encode(lux);

    portENTER_CRITICAL(&s_adaptive_mux);
    if (s_lux_history.day_key != day_key)
    {
        history_finish_hour_locked();
        lux_history_advance_day(s_lux_history.today, s_lux_history.yesterday,
                                &s_lux_history.day_key, day_key);
        s_hour_sum_x10 = 0;
        s_hour_count = 0;
        s_hour = -1;
        s_lux_history.crc = history_crc32(&s_lux_history);
    }

    if (s_hour != hour)
    {
        history_finish_hour_locked();
        s_hour_sum_x10 = 0;
        s_hour_count = 0;
        s_hour = hour;
    }

    if (s_hour_count < UINT16_MAX)
    {
        s_hour_sum_x10 += sample_x10;
        s_hour_count++;
    }
    portEXIT_CRITICAL(&s_adaptive_mux);
}

static uint8_t time_brightness_percent(uint8_t profile)
{
    if (profile > 1U)
        profile = 0U;
    uint8_t percent = eeprom_brightness_LED[profile];
    if (percent < ADAPT_CURVE_MIN_LEVEL)
        percent = ADAPT_CURVE_MIN_LEVEL;
    if (percent > ADAPT_CURVE_MAX_LEVEL)
        percent = ADAPT_CURVE_MAX_LEVEL;
    return percent;
}

void adaptive_init(void)
{
    if (!adaptive_curve_valid(&eeprom_adapt_curve))
    {
        adaptive_curve_factory_default();
        ESP_LOG_WEB(ESP_LOG_WARN, TAG,
                    "Invalid stored brightness curve; restored factory defaults");
    }

    adaptive_curve_prepare_log10(&eeprom_adapt_curve, s_curve_log_lux);
    s_hour_sum_x10 = 0;
    s_hour_count = 0;
    s_hour = -1;
    s_current_log_lux = adaptive_lux_to_log10(ADAPT_SENSOR_MIN_LUX);
    s_state = ADAPT_STATE_INITIALIZED;

    const esp_reset_reason_t reset_reason = esp_reset_reason();
    bool reset_history = false;
    portENTER_CRITICAL(&s_adaptive_mux);
    if (reset_reason == ADAPT_RTC_RESET_REASON_POWER_ON ||
        reset_reason == ADAPT_RTC_RESET_REASON_BROWNOUT ||
        !history_valid())
    {
        history_reset();
        reset_history = true;
    }
    portEXIT_CRITICAL(&s_adaptive_mux);
    if (reset_history)
        ESP_LOG_WEB(ESP_LOG_INFO, TAG,
                    "Ambient light history initialized (reset reason %d)",
                    (int)reset_reason);
}

bool adaptive_curve_set(const adaptive_curve_t *curve)
{
    if (!adaptive_curve_valid(curve))
        return false;

    portENTER_CRITICAL(&s_adaptive_mux);
    eeprom_adapt_curve = *curve;
    adaptive_curve_prepare_log10(&eeprom_adapt_curve, s_curve_log_lux);
    portEXIT_CRITICAL(&s_adaptive_mux);
    return true;
}

bool adaptive_curve_get(adaptive_curve_t *curve)
{
    if (curve == NULL)
        return false;

    portENTER_CRITICAL(&s_adaptive_mux);
    *curve = eeprom_adapt_curve;
    portEXIT_CRITICAL(&s_adaptive_mux);
    return adaptive_curve_valid(curve);
}

void adaptive_curve_factory_default(void)
{
    adaptive_curve_t curve;
    adaptive_curve_build_factory_default(&curve);
    if (!adaptive_curve_set(&curve))
    {
        ESP_LOG_WEB(ESP_LOG_ERROR, TAG,
                    "Could not create the factory adaptive brightness curve");
    }
}

uint8_t brightness_target_percent(void)
{
    if (manufacturer_mode || eeprom_dim_mode == DIM_MODE_FULL)
        return ADAPT_CURVE_MAX_LEVEL;

    if (eeprom_dim_mode == DIM_MODE_TIME)
        return time_brightness_percent(font_index);

    adaptive_curve_t curve;
    int16_t log_lux[ADAPT_CURVE_POINTS];
    int16_t current_log_lux;
    bool has_lux;

    portENTER_CRITICAL(&s_adaptive_mux);
    curve = eeprom_adapt_curve;
    memcpy(log_lux, s_curve_log_lux, sizeof(log_lux));
    current_log_lux = s_current_log_lux;
    has_lux = (s_state & ADAPT_STATE_HAS_LUX) != 0;
    portEXIT_CRITICAL(&s_adaptive_mux);

    if (!adaptive_curve_valid(&curve))
        return ADAPT_CURVE_MAX_LEVEL;
    if (!has_lux)
        return curve.pct[0];

    return adaptive_curve_percent_at_log10(&curve, log_lux, current_log_lux);
}

void adaptive_tick(float lux)
{
    if ((s_state & ADAPT_STATE_INITIALIZED) == 0)
        adaptive_init();

    if (!isfinite(lux))
        return;

    const int16_t current_log_lux = adaptive_lux_to_log10(lux);
    portENTER_CRITICAL(&s_adaptive_mux);
    s_current_log_lux = current_log_lux;
    s_state |= ADAPT_STATE_HAS_LUX;
    portEXIT_CRITICAL(&s_adaptive_mux);

    history_sample(lux);

    if (manufacturer_mode || eeprom_dim_mode != DIM_MODE_BRIGHTNESS)
    {
        portENTER_CRITICAL(&s_adaptive_mux);
        s_applied_brightness = pwm_get_current_brightness();
        s_state |= ADAPT_STATE_HAS_APPLIED;
        portEXIT_CRITICAL(&s_adaptive_mux);
        return;
    }

    const uint8_t target = brightness_target_percent();
    uint8_t applied = pwm_get_current_brightness();
    bool has_applied;

    portENTER_CRITICAL(&s_adaptive_mux);
    has_applied = (s_state & ADAPT_STATE_HAS_APPLIED) != 0;
    if (!has_applied)
    {
        s_applied_brightness = target;
        s_state |= ADAPT_STATE_HAS_APPLIED;
    }
    else if (applied != s_applied_brightness)
    {
        s_applied_brightness = applied;
    }

    if (target > s_applied_brightness)
    {
        const uint8_t distance = (uint8_t)(target - s_applied_brightness);
        s_applied_brightness += distance > ADAPT_MAX_STEP_PCT
                                    ? ADAPT_MAX_STEP_PCT
                                    : distance;
    }
    else if (target < s_applied_brightness)
    {
        const uint8_t distance = (uint8_t)(s_applied_brightness - target);
        s_applied_brightness -= distance > ADAPT_MAX_STEP_PCT
                                    ? ADAPT_MAX_STEP_PCT
                                    : distance;
    }

    applied = s_applied_brightness;
    portEXIT_CRITICAL(&s_adaptive_mux);

    if (applied != pwm_get_current_brightness())
        set_led_pwm_brightness_quiet(applied);
}

int lux_history_write_json(char *buf, size_t len)
{
    if (buf == NULL || len == 0)
        return -1;

    int hour = -1;
    int32_t day_key = LUX_HIST_INVALID_DAY;
    if (time_valid)
    {
        time_t now;
        struct tm local_time;
        time(&now);
        if (localtime_r(&now, &local_time) != NULL)
        {
            hour = local_time.tm_hour;
            day_key = local_day_key(&local_time);
        }
    }

    uint16_t today[LUX_HIST_HOURS];
    uint16_t yesterday[LUX_HIST_HOURS];
    portENTER_CRITICAL(&s_adaptive_mux);
    memcpy(today, s_lux_history.today, sizeof(today));
    memcpy(yesterday, s_lux_history.yesterday, sizeof(yesterday));
    if (hour >= 0 && hour < LUX_HIST_HOURS &&
        s_lux_history.day_key == day_key &&
        s_hour == hour && s_hour_count > 0)
        today[hour] = lux_history_average(s_hour_sum_x10, s_hour_count);
    portEXIT_CRITICAL(&s_adaptive_mux);
    return lux_history_format_json(buf, len, hour, today, yesterday);
}
