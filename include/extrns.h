#ifndef EXTRNS_H
#define EXTRNS_H

// keyboard driver
void keyboard_init();
char getchar();

// tty driver
void cleark();
void putck(char c);
void putsk(const char *str);

#endif // EXTRNS_H