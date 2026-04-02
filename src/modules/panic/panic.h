#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>
#include <stddef.h>

void panic(const char *message);
void test_panic(void);

#endif