#ifndef F_ADAPTIVE_H
#define F_ADAPTIVE_H

#include "f-adaptive-core.h"

#include <stddef.h>

void adaptive_init(void);
bool adaptive_curve_set(const adaptive_curve_t *curve);
bool adaptive_curve_get(adaptive_curve_t *curve);
void adaptive_curve_factory_default(void);
uint8_t brightness_target_percent(void);
void adaptive_tick(float lux);
int lux_history_write_json(char *buf, size_t len);

#endif
