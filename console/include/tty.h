#ifndef TTY_H
#define TTY_H

#include "common.h"

#define MAX_TTY_CHARS 1024

void tty_init(void);
void tty_update(rastislav_t *rastislav);

#endif // TTY_H