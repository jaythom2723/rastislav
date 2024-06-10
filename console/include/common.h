#ifndef COMMON_H
#define COMMON_H

#define RAX             0
#define RBX             1
#define RCX             2
#define RDX             3
#define REX             4
#define RFX             5
#define RGX             6
#define RIX             7
#define RSX             8
#define RTX             9
#define RMX             10
#define RPX             11
#define RKX             12
#define RVX             13
#define RRX             14
#define RHX             15
#define RLX             16

#define PROGRAM_LO      0
#define PROGRAM_HI      16384
#define DATA_LO         16385
#define DATA_HI         32768
#define STACK_LO        32769
#define STACK_HI        49152
#define VIDEO_LO        49153
#define VIDEO_HI        65536

#define TELETYPE_PORT   0
#define VIDEO_PORT_X    1
#define VIDEO_PORT_Y    2
#define AUDIO_PORT      3
#define PLAYER_1_PORT   4
#define PLAYER_2_PORT   5

#define WIDTH 800
#define HEIGHT 600

#define NUM_INSTRUCTIONS 45

const static char characterSet[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '!', '@', '#', '$', '^', '&', '*', '(', ')', '-', '_', '+',
    '=', '`', '~', '.', ',', '/', '?', ';', ':', '\'', '"', '[', ']', '\\', '|', '}',
    '{', '<', '>', '\n', '\t', ' '
};

typedef enum
{
    ADD = 80,
    MUL,
    ARV,
    MRV,
    ADA,
    MLA,
    SUB = 96,
    DIV,
    SRV,
    DRV,
    SBA,
    DVA,
    XOR = 128,
    XRV,
    XRA,
    OR = 144,
    ORV,
    ORA,
    AND = 160,
    CMPR,
    CMPA,
    CMPV,
    ANDA,
    ANDV
} alu_ops_t;

typedef enum
{
    JMP, JNC, JC, JNZ, JZ
} jmp_ops_t;

typedef enum
{
    WTP, WRVP
} write_ops_t;

typedef enum
{
    ZERO    = 1,
    NOT     = 2,
    CARRY   = 4,
    TTY_OUT = 8,
    VID_OUT = 16,
    AUD_OUT = 32,
    GREATER = 64,
    GREATER_EQUAL = 128,
    LESSER  = 256,
    LESSER_EQUAL = 512,
    EQUAL = 1024,
} flags_t;

typedef struct
{
    unsigned short regs[RLX+1];
    unsigned char mem[65536];
    unsigned char rom[1024];

    unsigned short ports[PLAYER_2_PORT + 1];

    unsigned char is_processing : 1;
    unsigned char is_on : 1;
    unsigned char is_playing : 1;
    unsigned char jump : 1;
} rastislav_t;

typedef struct
{
    unsigned char regs[RKX+1];

    unsigned short address;
    unsigned short value;

    union
    {
        alu_ops_t alu_operation;
        jmp_ops_t jmp_operation;
        write_ops_t write_operation;
    };
} args_t;

typedef struct
{
    int argc;
    char *name;
    unsigned char hibyte;
    void (*callback)(rastislav_t *rastislav, args_t args);
} instruction_t;

void scream(const char *msg);

#endif // COMMON_H