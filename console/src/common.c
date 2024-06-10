#include "common.h"

#include <stdio.h>
#include <stdlib.h>

void scream(const char *msg)
{
    int i = 0;

    for(; i < 10; ++i)
    {
        printf("!!!!!!!!!!%s!!!!!!!!!!\n", msg);
    }

    exit(-1);
}