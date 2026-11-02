#include "f-adaptive-core.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define ADAPT_MIN_LUX 0.1f
#define ADAPT_MAX_LUX 6553.0f

bool adaptive_curve_valid(const adaptive_curve_t *curve)
{
    if (curve == NULL)
        return false;

    for (size_t i = 0; i < ADAPT_CURVE_POINTS; i++)
    {
        if (curve->lux_x10[i] < ADAPT_CURVE_MIN_LUX_X10 ||
            curve->lux_x10[i] > ADAPT_CURVE_MAX_LUX_X10 ||
            curve->pct[i] < ADAPT_CURVE_MIN_LEVEL ||
            curve->pct[i] > ADAPT_CURVE_MAX_LEVEL)
            return false;

        if (i > 0 &&
            (uint64_t)curve->lux_x10[i] * ADAPT_CURVE_MIN_GAP_DENOMINATOR <
                (uint64_t)curve->lux_x10[i - 1] * ADAPT_CURVE_MIN_GAP_NUMERATOR)
            return false;
    }

    return true;
}

void adaptive_curve_prepare_log10(const adaptive_curve_t *curve,
                                  int16_t log_lux[ADAPT_CURVE_POINTS])
{
    if (curve == NULL || log_lux == NULL)
        return;
    if (!adaptive_curve_valid(curve))
    {
        memset(log_lux, 0, sizeof(int16_t) * ADAPT_CURVE_POINTS);
        return;
    }

    for (size_t i = 0; i < ADAPT_CURVE_POINTS; i++)
    {
        const float lux = (float)curve->lux_x10[i] / (float)LUX_HIST_SCALE;
        const long scaled = lroundf(log10f(lux) * (float)ADAPT_LOG10_SCALE);
        log_lux[i] = (int16_t)scaled;
    }
}

int16_t adaptive_lux_to_log10(float lux)
{
    if (!isfinite(lux) || lux < ADAPT_MIN_LUX)
        lux = ADAPT_MIN_LUX;
    else if (lux > ADAPT_MAX_LUX)
        lux = ADAPT_MAX_LUX;

    const long scaled = lroundf(log10f(lux) * (float)ADAPT_LOG10_SCALE);
    if (scaled < INT16_MIN)
        return INT16_MIN;
    if (scaled > INT16_MAX)
        return INT16_MAX;
    return (int16_t)scaled;
}

uint8_t adaptive_curve_percent_at_log10(const adaptive_curve_t *curve,
                                        const int16_t log_lux[ADAPT_CURVE_POINTS],
                                        int16_t current_log_lux)
{
    if (!adaptive_curve_valid(curve) || log_lux == NULL)
        return ADAPT_CURVE_MAX_LEVEL;

    if (current_log_lux <= log_lux[0])
        return curve->pct[0];
    if (current_log_lux >= log_lux[ADAPT_CURVE_POINTS - 1])
        return curve->pct[ADAPT_CURVE_POINTS - 1];

    for (size_t i = 0; i < ADAPT_CURVE_POINTS - 1; i++)
    {
        if (current_log_lux <= log_lux[i + 1])
        {
            const float span = (float)(log_lux[i + 1] - log_lux[i]);
            if (span <= 0.0f)
                return curve->pct[i + 1];

            const float position = (float)(current_log_lux - log_lux[i]) / span;
            const float level = (float)curve->pct[i] +
                                ((float)curve->pct[i + 1] - (float)curve->pct[i]) * position;
            long rounded = lroundf(level);
            if (rounded < ADAPT_CURVE_MIN_LEVEL)
                rounded = ADAPT_CURVE_MIN_LEVEL;
            if (rounded > ADAPT_CURVE_MAX_LEVEL)
                rounded = ADAPT_CURVE_MAX_LEVEL;
            return (uint8_t)rounded;
        }
    }

    return curve->pct[ADAPT_CURVE_POINTS - 1];
}

uint8_t adaptive_curve_percent_for_lux(const adaptive_curve_t *curve, float lux)
{
    if (!adaptive_curve_valid(curve))
        return ADAPT_CURVE_MAX_LEVEL;

    int16_t log_lux[ADAPT_CURVE_POINTS];
    adaptive_curve_prepare_log10(curve, log_lux);
    return adaptive_curve_percent_at_log10(curve, log_lux, adaptive_lux_to_log10(lux));
}

void adaptive_curve_build_factory_default(adaptive_curve_t *curve)
{
    if (curve == NULL)
        return;

    static const uint16_t default_lux_x10[ADAPT_CURVE_POINTS] = {12U, 32U, 310U, 1300U};
    static const uint8_t default_percent[ADAPT_CURVE_POINTS] = {2U, 44U, 55U, 76U};
    memcpy(curve->lux_x10, default_lux_x10, sizeof(default_lux_x10));
    memcpy(curve->pct, default_percent, sizeof(default_percent));
}

bool adaptive_threshold_valid(float threshold, float sensitivity)
{
    return isfinite(threshold) && isfinite(sensitivity) &&
           threshold >= 0.0f && threshold <= ADAPT_MAX_THRESHOLD_LUX &&
           sensitivity >= 0.0f && sensitivity <= ADAPT_MAX_SENSITIVITY_LUX &&
           sensitivity <= threshold;
}

void lux_history_clear(uint16_t today[LUX_HIST_HOURS],
                       uint16_t yesterday[LUX_HIST_HOURS])
{
    if (today != NULL)
        for (size_t i = 0; i < LUX_HIST_HOURS; i++)
            today[i] = LUX_HIST_EMPTY;

    if (yesterday != NULL)
        for (size_t i = 0; i < LUX_HIST_HOURS; i++)
            yesterday[i] = LUX_HIST_EMPTY;
}

void lux_history_advance_day(uint16_t today[LUX_HIST_HOURS],
                             uint16_t yesterday[LUX_HIST_HOURS],
                             int32_t *stored_day, int32_t current_day)
{
    if (today == NULL || yesterday == NULL || stored_day == NULL ||
        *stored_day == current_day)
        return;

    if (*stored_day != LUX_HIST_INVALID_DAY && current_day != INT32_MIN &&
        *stored_day == current_day - 1)
        memcpy(yesterday, today, sizeof(uint16_t) * LUX_HIST_HOURS);
    else
        for (size_t i = 0; i < LUX_HIST_HOURS; i++)
            yesterday[i] = LUX_HIST_EMPTY;

    for (size_t i = 0; i < LUX_HIST_HOURS; i++)
        today[i] = LUX_HIST_EMPTY;
    *stored_day = current_day;
}

uint16_t lux_history_encode(float lux)
{
    if (!isfinite(lux) || lux <= 0.0f)
        return 0;
    if (lux >= (float)LUX_HIST_MAX_STORED / (float)LUX_HIST_SCALE)
        return LUX_HIST_MAX_STORED;

    const long stored = lroundf(lux * (float)LUX_HIST_SCALE);
    if (stored < 0)
        return 0;
    if (stored > (long)LUX_HIST_MAX_STORED)
        return LUX_HIST_MAX_STORED;
    return (uint16_t)stored;
}

uint16_t lux_history_average(uint32_t sum_x10, uint16_t count)
{
    if (count == 0)
        return LUX_HIST_EMPTY;

    uint64_t average = ((uint64_t)sum_x10 + (uint32_t)count / 2U) / (uint32_t)count;
    if (average > LUX_HIST_MAX_STORED)
        average = LUX_HIST_MAX_STORED;
    return (uint16_t)average;
}

int lux_history_format_json(char *buf, size_t len, int hour,
                            const uint16_t today[LUX_HIST_HOURS],
                            const uint16_t yesterday[LUX_HIST_HOURS])
{
    if (buf == NULL || len == 0 || today == NULL || yesterday == NULL ||
        hour < -1 || hour >= LUX_HIST_HOURS)
        return -1;

    int used = snprintf(buf, len, "{\"h\":%d,\"t\":[", hour);
    if (used < 0 || (size_t)used >= len)
        return -1;

    for (size_t i = 0; i < LUX_HIST_HOURS; i++)
    {
        const int written = snprintf(buf + used, len - (size_t)used,
                                     "%s%d", i == 0 ? "" : ",",
                                     today[i] == LUX_HIST_EMPTY ? -1 : (int)today[i]);
        if (written < 0 || (size_t)written >= len - (size_t)used)
            return -1;
        used += written;
    }

    int written = snprintf(buf + used, len - (size_t)used, "],\"y\":[");
    if (written < 0 || (size_t)written >= len - (size_t)used)
        return -1;
    used += written;

    for (size_t i = 0; i < LUX_HIST_HOURS; i++)
    {
        written = snprintf(buf + used, len - (size_t)used,
                           "%s%d", i == 0 ? "" : ",",
                           yesterday[i] == LUX_HIST_EMPTY ? -1 : (int)yesterday[i]);
        if (written < 0 || (size_t)written >= len - (size_t)used)
            return -1;
        used += written;
    }

    written = snprintf(buf + used, len - (size_t)used, "]}");
    if (written < 0 || (size_t)written >= len - (size_t)used)
        return -1;
    return used + written;
}
