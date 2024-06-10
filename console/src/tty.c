#include "tty.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char tty_buffer[MAX_TTY_CHARS] = { 0 };
char *tty_buf_ptr;

void tty_init(void)
{
    memset(tty_buffer, 0, MAX_TTY_CHARS);
    tty_buf_ptr = tty_buffer;
}

void tty_update(rastislav_t *rastislav)
{
    // Steps for success
    // 1: Check for proper flag
    // 2: Check if the buffer has exceeded character limit
    //      2.a: Scream if true
    // 3: cache the teletype port value
    // 4: reset teletype port
    // 5: push character to buffer
    unsigned char character = 0;

    if(rastislav->regs[RFX] != TTY_OUT)
        return;

    if((tty_buf_ptr - tty_buffer) >= MAX_TTY_CHARS)
        scream("TELETYPE BUFFER OVERFLOW");
    
    character = characterSet[rastislav->ports[TELETYPE_PORT] & 0x00FF];
    rastislav->ports[TELETYPE_PORT] = 0;

    (*tty_buf_ptr) = character;
    tty_buf_ptr++;
}