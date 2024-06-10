#include <stdio.h>
#include <conio.h>
#include <Windows.h>

#include "console.h"
#include "video.h"
#include "tty.h"

void debugConsole(rastislav_t console, int step);

int main(void)
{
    rastislav_t rastislav = { 0 };
    
    unsigned char program[16384] = {
        0x70, 0x00, 0x08,                   // lrv rax, 8
        0x50, 0x00,                         // add rax, rax     8 + 8 = 16
        0x51, 0x00,                         // mul rax, rax     16 * 16 = 256
        0x52, 0x00, 0x00, 0x01,             // add rax, 1       256 + 1 = 257
        0x53, 0x00, 0x00, 0x01,             // mul rax, 1       257 * 1 = 257
        0x54, 0x00, 0x82, 0xA5,             // add rax, $82A5   257 + 0 = 257
        0x55, 0x00, 0x82, 0xA5,             // mul rax, $82A5   257 * 0 = 0
        0x30,                               // hlt
    };

    createRastislav(&rastislav);
    videoInit(&rastislav);
    tty_init();
    loadProgram(&rastislav, program);

    while(rastislav.is_on == 1 && (rastislav.is_processing == 1 || rastislav.is_playing == 1))
    {
        rastislavProcess(&rastislav);
        tty_update(&rastislav);
        videoUpdateConsoleReference(&rastislav);
        videoProcess(&rastislav);

        Sleep(100);
        debugConsole(rastislav, TRUE);
    }

    while(rastislav.is_on == 1)
    {
        printf("RASTISLAV IS ON BUT NOT PROCESSING...\n");
        Sleep(500);
    }

    destroyRastislav(&rastislav);

    return 0;
}

void debugConsole(rastislav_t rastislav, int step)
{
    int i = 0, j = 0;
    char c = ' '; // register identification character
    char regChars[] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'i', 's', 't', 'm', 'p', 'k', 'v',
        'r', 'h', 'l'
    };
    const char *portNames[] = {
        "TTY_OUT", "VID_X_IN", "VID_Y_IN", "VID_DATA_IN", "AUD_OUT", "PLY_1_IN", "PLY_2_IN" 
    };

    printf("-=-=-=-=-=-=-=-\tSTACK (1ST 5)\t-=-=-=-=-=-=-=-\n");
    for(i = STACK_LO; i < STACK_LO + 5; ++i)
    {
        printf("STACK[%d] ===> %d\n", (i - STACK_LO), rastislav.mem[i]);
    }

    printf("-=-=-=-=-=-=-=-\tREGISTERS\t-=-=-=-=-=-=-=-\n");
    for(i = 0; i < RLX+1; ++i)
    {
        printf("[REG] r%cx\t\t\t\t===> 0x%04x\n", regChars[i], rastislav.regs[i]);
    }

    printf("-=-=-=-=-=-=-=-\tPORTS\t-=-=-=-=-=-=-=-\n");
    for(i = 0; i < PLAYER_2_PORT+1; ++i)
    {
        printf("[PORT] %s_PORT\t\t\t===> 0x%04x\n", portNames[i], rastislav.ports[i]);
    }

    if(step == 1)
        getch();
}