#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void init_time(void);
uint64_t get_time_ms(void);
void delay(uint32_t ms);

#endif