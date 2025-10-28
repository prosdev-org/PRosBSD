#ifndef TIMER_H
#define TIMER_H

void timer_tick();

unsigned long long timer_nanos_since_boot();

long double timer_seconds_since_boot();

#endif
