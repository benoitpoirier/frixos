#ifndef F_TIME_H
#define F_TIME_H    // Guard to prevent multiple inclusion

#include <stdbool.h>
#include "time.h"

int get_moon_index();
void get_current_time(int *hour, int *minute);
void sync_time_with_ntp(void);
bool is_hour_in_range(uint8_t start_hour, uint8_t end_hour, int current_hour);


#endif