#ifndef CONSOLE_H
#define CONSOLE_H

#include "common.h"

// TODO: Make sure to double check if the instructions being used are used registers
// or real values now. Easy fix to prevent leaks.

void store(rastislav_t *rastislav, args_t args);
void read(rastislav_t *rastislav, args_t args);
void halt(rastislav_t *rastislav, args_t args);
void move(rastislav_t *rastislav, args_t args);
void alu(rastislav_t *rastislav, args_t args);
void jmp(rastislav_t *rastislav, args_t args);
void lrv(rastislav_t *rastislav, args_t args);
void readPort(rastislav_t *rastislav, args_t args);
void writePort(rastislav_t *rastislav, args_t args);
void push(rastislav_t *rastislav, args_t args);
void pop(rastislav_t *rastislav, args_t args);

// the following list of constants are for the sake of my sanity in order
// to read the code without having to memorize bytecode segments.
static const unsigned char              MC_STR      =       0x10;             // store
static const unsigned char              MC_RD       =       0x20;             // read
static const unsigned char              MC_HLT      =       0x30;             // halt
static const unsigned char              MC_RDP      =       0x31;             // read port
static const unsigned char              MC_WTP      =       0x32;             // write port
static const unsigned char              MC_WRVP     =       0x33;             // write real value to port
static const unsigned char              MC_MOV      =       0x40;             // move
static const unsigned char              MC_PSH      =       0x41;             // push
static const unsigned char              MC_POP      =       0x42;             // pop
static const unsigned char              MC_ADD      =       0x50;             // add
static const unsigned char              MC_MUL      =       0x51;             // multiply
static const unsigned char              MC_ARV      =       0x52;             // add real value
static const unsigned char              MC_MRV      =       0x53;             // multiply real value
static const unsigned char              MC_ADA      =       0x54;             // add address
static const unsigned char              MC_MLA      =       0x55;             // multiply address
static const unsigned char              MC_SUB      =       0x60;             // subtract
static const unsigned char              MC_DIV      =       0x61;             // divide
static const unsigned char              MC_SRV      =       0x62;             // subtract real value
static const unsigned char              MC_DRV      =       0x63;             // divide real value
static const unsigned char              MC_SBA      =       0x64;             // subtract address
static const unsigned char              MC_DVA      =       0x65;             // divide address
static const unsigned char              MC_LRV      =       0x70;             // load real value
static const unsigned char              MC_XOR      =       0x80;             // exclusive bitwise or
static const unsigned char              MC_XRV      =       0x81;             // bitwise xor value
static const unsigned char              MC_XRA      =       0x82;             // bitwise xor address
static const unsigned char              MC_OR       =       0x90;             // bitwise or
static const unsigned char              MC_ORV      =       0x91;             // bitwise or value
static const unsigned char              MC_ORA      =       0x92;             // bitwise or address
static const unsigned char              MC_AND      =       0xA0;             // bitwise and
static const unsigned char              MC_CMPR     =       0xA1;             // compare registers
static const unsigned char              MC_CMPV     =       0xA2;             // compare values
static const unsigned char              MC_CMPA     =       0xA3;             // compare addresses
static const unsigned char              MC_ANDV     =       0xA4;             // bitwise and value
static const unsigned char              MC_ANDA     =       0xA5;             // bitwise and address
static const unsigned char              MC_JMP      =       0xB0;             // jump
static const unsigned char              MC_JG       =       0xB1;             // jump >
static const unsigned char              MC_JGE      =       0xB2;             // jump >=
static const unsigned char              MC_JL       =       0xB3;             // jump <
static const unsigned char              MC_JLE      =       0xB4;             // jump <=
static const unsigned char              MC_JEQ      =       0xB5;             // jump ==
static const unsigned char              MC_JNE      =       0xB6;             // jump !=
static const unsigned char              MC_JZ       =       0xC0;             // jump if zero
static const unsigned char              MC_JC       =       0xD0;             // jump if carry
static const unsigned char              MC_JNZ      =       0xE0;             // jump if not zero
static const unsigned char              MC_JNC      =       0xF0;             // jump if not carry

static const instruction_t instructions[] = {
    { .argc=2,      .name="STR",         .hibyte=0x10,          .callback=&store },
    { .argc=2,      .name="RD",          .hibyte=0x20,          .callback=&read },
    { .argc=0,      .name="HLT",         .hibyte=0x30,          .callback=&halt },
    { .argc=1,      .name="RDP",         .hibyte=0x31,          .callback=&readPort },
    { .argc=1,      .name="WTP",         .hibyte=0x32,          .callback=&writePort },
    { .argc=1,      .name="WRVP",        .hibyte=0x33,          .callback=&writePort },
    { .argc=2,      .name="MOV",         .hibyte=0x40,          .callback=&move },
    { .argc=1,      .name="PSH",         .hibyte=0x41,          .callback=&push },
    { .argc=1,      .name="POP",         .hibyte=0x42,          .callback=&pop },
    { .argc=2,      .name="ADD",         .hibyte=0x50,          .callback=&alu },
    { .argc=2,      .name="MUL",         .hibyte=0x51,          .callback=&alu },
    { .argc=2,      .name="ARV",         .hibyte=0x52,          .callback=&alu },
    { .argc=2,      .name="MRV",         .hibyte=0x53,          .callback=&alu },
    { .argc=2,      .name="ADA",         .hibyte=0x54,          .callback=&alu },
    { .argc=2,      .name="MLA",         .hibyte=0x55,          .callback=&alu },
    { .argc=2,      .name="SUB",         .hibyte=0x60,          .callback=&alu },
    { .argc=2,      .name="DIV",         .hibyte=0x61,          .callback=&alu },
    { .argc=2,      .name="SRV",         .hibyte=0x62,          .callback=&alu },
    { .argc=2,      .name="DRV",         .hibyte=0x63,          .callback=&alu },
    { .argc=2,      .name="SBA",         .hibyte=0x64,          .callback=&alu },
    { .argc=2,      .name="DVA",         .hibyte=0x65,          .callback=&alu },
    { .argc=2,      .name="LRV",         .hibyte=0x70,          .callback=&lrv },
    { .argc=2,      .name="XOR",         .hibyte=0x80,          .callback=&alu },
    { .argc=2,      .name="XRV",         .hibyte=0x81,          .callback=&alu },
    { .argc=2,      .name="XRA",         .hibyte=0x82,          .callback=&alu },
    { .argc=2,      .name="OR",          .hibyte=0x90,          .callback=&alu },
    { .argc=2,      .name="ORV",         .hibyte=0x91,          .callback=&alu },
    { .argc=2,      .name="ORA",         .hibyte=0x92,          .callback=&alu },
    { .argc=2,      .name="AND",         .hibyte=0xA0,          .callback=&alu },
    { .argc=2,      .name="CMPR",        .hibyte=0xA1,          .callback=&alu },
    { .argc=2,      .name="CMPV",        .hibyte=0xA2,          .callback=&alu },
    { .argc=2,      .name="CMPA",        .hibyte=0xA3,          .callback=&alu },
    { .argc=2,      .name="ANDV",        .hibyte=0xA4,          .callback=&alu },
    { .argc=2,      .name="ANDA",        .hibyte=0xA5,          .callback=&alu },
    { .argc=1,      .name="JMP",         .hibyte=0xB0,          .callback=&jmp },
    { .argc=1,      .name="JG",          .hibyte=0xB1,          .callback=&jmp },
    { .argc=1,      .name="JGE",         .hibyte=0xB2,          .callback=&jmp },
    { .argc=1,      .name="JL",          .hibyte=0xB3,          .callback=&jmp },
    { .argc=1,      .name="JLE",         .hibyte=0xB4,          .callback=&jmp },
    { .argc=1,      .name="JEQ",         .hibyte=0xB5,          .callback=&jmp },
    { .argc=1,      .name="JNE",         .hibyte=0xB6,          .callback=&jmp },
    { .argc=1,      .name="JZ",          .hibyte=0xC0,          .callback=&jmp },
    { .argc=1,      .name="JC",          .hibyte=0xD0,          .callback=&jmp },
    { .argc=1,      .name="JNZ",         .hibyte=0xE0,          .callback=&jmp },
    { .argc=1,      .name="JNC",         .hibyte=0xF0,          .callback=&jmp },
};

void createRastislav(rastislav_t *rastislav);
void destroyRastislav(rastislav_t *rastislav);
unsigned short getRegister(rastislav_t *rastislav, int index);
void loadProgram(rastislav_t *rastislav, unsigned char *program);
void rastislavProcess(rastislav_t *rastislav);

#endif // CONSOLE_H