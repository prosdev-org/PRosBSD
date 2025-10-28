#ifndef PIT_H
#define PIT_H

#define PIT_TARGET_FREQUENCY 100
#define PIT_BASE_FREQUENCY   1193180
#define PIT_DIVISOR          (PIT_BASE_FREQUENCY / PIT_TARGET_FREQUENCY)
#define PIT_ACTUAL_FREQUENCY ((long double) PIT_BASE_FREQUENCY / (long double) PIT_DIVISOR)

void pit_init();

#endif
