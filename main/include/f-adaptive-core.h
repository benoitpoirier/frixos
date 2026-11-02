#ifndef F_ADAPTIVE_CORE_H
#define F_ADAPTIVE_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ADAPT_CURVE_POINTS 4
#define ADAPT_CURVE_MIN_LUX_X10 1U
#define ADAPT_CURVE_MAX_LUX_X10 65530U
#define ADAPT_CURVE_MIN_LEVEL 1U
#define ADAPT_CURVE_MAX_LEVEL 100U
#define ADAPT_LOG10_SCALE 4096
#define ADAPT_CURVE_MIN_GAP_NUMERATOR 125893U
#define ADAPT_CURVE_MIN_GAP_DENOMINATOR 100000U
#define ADAPT_MAX_THRESHOLD_LUX 500.0f
#define ADAPT_MAX_SENSITIVITY_LUX 50.0f
#define ADAPT_DEFAULT_THRESHOLD_LUX 2.0f
#define ADAPT_DEFAULT_SENSITIVITY_LUX 0.5f
#define LUX_HIST_HOURS 24
#define LUX_HIST_EMPTY UINT16_MAX
#define LUX_HIST_SCALE 10U
#define LUX_HIST_MAX_STORED (UINT16_MAX - 1U)
#define LUX_HIST_INVALID_DAY INT32_MIN

typedef struct
{
    uint16_t lux_x10[ADAPT_CURVE_POINTS];
    uint8_t pct[ADAPT_CURVE_POINTS];
} adaptive_curve_t;

bool adaptive_curve_valid(const adaptive_curve_t *curve);
void adaptive_curve_prepare_log10(const adaptive_curve_t *curve, int16_t log_lux[ADAPT_CURVE_POINTS]);
int16_t adaptive_lux_to_log10(float lux);
uint8_t adaptive_curve_percent_at_log10(const adaptive_curve_t *curve,
                                        const int16_t log_lux[ADAPT_CURVE_POINTS],
                                        int16_t current_log_lux);
uint8_t adaptive_curve_percent_for_lux(const adaptive_curve_t *curve, float lux);
void adaptive_curve_build_factory_default(adaptive_curve_t *curve);
bool adaptive_threshold_valid(float threshold, float sensitivity);

void lux_history_clear(uint16_t today[LUX_HIST_HOURS], uint16_t yesterday[LUX_HIST_HOURS]);
void lux_history_advance_day(uint16_t today[LUX_HIST_HOURS],
                             uint16_t yesterday[LUX_HIST_HOURS],
                             int32_t *stored_day, int32_t current_day);
uint16_t lux_history_encode(float lux);
uint16_t lux_history_average(uint32_t sum_x10, uint16_t count);
int lux_history_format_json(char *buf, size_t len, int hour,
                            const uint16_t today[LUX_HIST_HOURS],
                            const uint16_t yesterday[LUX_HIST_HOURS]);

#endif
