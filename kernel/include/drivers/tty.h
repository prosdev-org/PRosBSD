#ifndef TTY_H
#define TTY_H

#include <stdint.h>

void cleark();
void putck(char c);
void putsk(const char *str);
void set_color(uint8_t fg, uint8_t bg);
void reset_color();

#endif
