#include <drivers/pit.h>
#include <timer.h>

static unsigned long long nanos = 0;

void timer_tick() {
    nanos += (long long) (1000000000.0L / PIT_ACTUAL_FREQUENCY);
}

unsigned long long timer_nanos_since_boot() {
    return nanos;
}

long double timer_seconds_since_boot() {
    return (long double) nanos / 1000000000.0;
}
