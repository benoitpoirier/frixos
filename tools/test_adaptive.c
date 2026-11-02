#include "f-adaptive-core.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define HISTORY_JSON_TEST_BUFFER_SIZE 384U

static size_t array_item_count(const char *json, const char *key)
{
    const char *start = strstr(json, key);
    assert(start != NULL);
    start = strchr(start, '[');
    assert(start != NULL);
    start++;
    size_t count = *start == ']' ? 0U : 1U;
    for (const char *p = start; *p != '\0' && *p != ']'; p++)
        if (*p == ',')
            count++;
    return count;
}

int main(void)
{
    const adaptive_curve_t curve = {
        .lux_x10 = {10, 100, 1000, 10000},
        .pct = {20, 40, 60, 80},
    };
    assert(adaptive_curve_valid(&curve));
    assert(adaptive_curve_percent_for_lux(&curve, 0.0f) == 20);
    assert(adaptive_curve_percent_for_lux(&curve, 1.0f) == 20);
    assert(adaptive_curve_percent_for_lux(&curve, 10.0f) == 40);
    assert(adaptive_curve_percent_for_lux(&curve, 100.0f) == 60);
    assert(adaptive_curve_percent_for_lux(&curve, 1000.0f) == 80);
    assert(adaptive_curve_percent_for_lux(&curve, 10000.0f) == 80);
    assert(adaptive_curve_percent_for_lux(&curve, sqrtf(1000.0f)) == 50);
    assert(adaptive_curve_percent_for_lux(&curve, NAN) == 20);

    adaptive_curve_t invalid = curve;
    invalid.lux_x10[1] = invalid.lux_x10[0];
    assert(!adaptive_curve_valid(&invalid));
    assert(adaptive_curve_percent_for_lux(&invalid, 10.0f) == ADAPT_CURVE_MAX_LEVEL);
    invalid = curve;
    invalid.lux_x10[1] = 12;
    assert(!adaptive_curve_valid(&invalid));
    invalid = curve;
    invalid.pct[2] = 0;
    assert(!adaptive_curve_valid(&invalid));

    adaptive_curve_t factory;
    adaptive_curve_build_factory_default(&factory);
    assert(adaptive_curve_valid(&factory));
    const uint16_t factory_lux_x10[ADAPT_CURVE_POINTS] = {12, 32, 310, 1300};
    const uint8_t factory_percent[ADAPT_CURVE_POINTS] = {2, 44, 55, 76};
    for (size_t i = 0; i < ADAPT_CURVE_POINTS; i++)
    {
        assert(factory.lux_x10[i] == factory_lux_x10[i]);
        assert(factory.pct[i] == factory_percent[i]);
    }

    assert(adaptive_threshold_valid(ADAPT_DEFAULT_THRESHOLD_LUX, ADAPT_DEFAULT_SENSITIVITY_LUX));
    assert(adaptive_threshold_valid(0.0f, 0.0f));
    assert(adaptive_threshold_valid(ADAPT_MAX_THRESHOLD_LUX, ADAPT_MAX_SENSITIVITY_LUX));
    assert(!adaptive_threshold_valid(5.0f, 6.0f));
    assert(!adaptive_threshold_valid(-1.0f, 0.0f));
    assert(!adaptive_threshold_valid(ADAPT_MAX_THRESHOLD_LUX + 1.0f, 0.5f));
    assert(!adaptive_threshold_valid(100.0f, ADAPT_MAX_SENSITIVITY_LUX + 1.0f));
    assert(!adaptive_threshold_valid(NAN, 0.5f));
    assert(!adaptive_threshold_valid(2.0f, NAN));

    uint16_t today[LUX_HIST_HOURS];
    uint16_t yesterday[LUX_HIST_HOURS];
    lux_history_clear(today, yesterday);
    for (int i = 0; i < LUX_HIST_HOURS; i++)
    {
        assert(today[i] == LUX_HIST_EMPTY);
        assert(yesterday[i] == LUX_HIST_EMPTY);
    }

    int32_t day = LUX_HIST_INVALID_DAY;
    lux_history_advance_day(today, yesterday, &day, 100);
    assert(day == 100);
    assert(today[0] == LUX_HIST_EMPTY);
    today[0] = 1234;
    lux_history_advance_day(today, yesterday, &day, 101);
    assert(day == 101);
    assert(yesterday[0] == 1234);
    assert(today[0] == LUX_HIST_EMPTY);
    today[5] = 456;
    lux_history_advance_day(today, yesterday, &day, 103);
    assert(day == 103);
    assert(yesterday[0] == LUX_HIST_EMPTY);
    assert(today[5] == LUX_HIST_EMPTY);

    assert(lux_history_encode(23.45f) == 235);
    assert(lux_history_encode(7000.0f) == LUX_HIST_MAX_STORED);
    assert(lux_history_encode(NAN) == 0);
    assert(lux_history_average(300, 2) == 150);
    assert(lux_history_average(0, 0) == LUX_HIST_EMPTY);

    today[0] = 123;
    yesterday[0] = 456;
    char history_json[HISTORY_JSON_TEST_BUFFER_SIZE];
    assert(lux_history_format_json(history_json, sizeof(history_json), 15,
                                   today, yesterday) > 0);
    assert(strstr(history_json, "{\"h\":15,\"t\":[123,-1") == history_json);
    assert(strstr(history_json, "],\"y\":[456,-1") != NULL);
    assert(array_item_count(history_json, "\"t\":") == LUX_HIST_HOURS);
    assert(array_item_count(history_json, "\"y\":") == LUX_HIST_HOURS);
    assert(lux_history_format_json(history_json, sizeof(history_json), -1,
                                   today, yesterday) > 0);
    assert(strstr(history_json, "{\"h\":-1,") == history_json);
    return 0;
}
