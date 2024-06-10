#include "console.h"

#include "common.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

// checking opcodes during argument determination
#define CHECK_ALU_REAL_VALUE_OP(hi) \
    hi == MC_ARV || hi == MC_MRV || hi == MC_SRV || hi == MC_DRV || \
    hi == MC_CMPV || hi == MC_XRV || hi == MC_ORV || hi == MC_ANDV

#define CHECK_ALU_ADDRESS_OP(hi) \
    hi == MC_ADA || hi == MC_MLA || hi == MC_SBA || hi == MC_DVA || \
    hi == MC_CMPA || hi == MC_XRA || hi == MC_ORA || hi == MC_ANDA



// checking enums during processing
#define IS_ALU_RV_OP(op) \
    op == ARV || op == MRV || op == SRV || op == DRV || \
    op == CMPV || op == XRV || op == ORV || op == ANDV

#define IS_ALU_ADDRESS_OP(op) \
    op == ADA || op == MLA || op == SBA || op == DVA || \
    op == CMPA || op == XRA || op == ORA || op == ANDA 

// TODO: test new instructions
//          ARV, MRV, ADA, MLA, SRV, DRV, SBA, DVA, XOR, XRV, XRA, OR, ORV, ORA, AND, ANDV, ANDA, CMPR, CMPV, CMPA, WRVP, JG, JGE, JLE, JL, JEQ, JNE

// TODO: Test video pixel output

// TODO: Test loops
// TODO: Test player 1 input
// TODO: Test player 2 input

unsigned char rom_const[1024] = { 0 };

args_t grabArguments(rastislav_t *rastislav, int *step, int pc, unsigned char hi, unsigned char lo);
unsigned int opcodeHandshake(rastislav_t *rastislav, int pc, unsigned char *hi, unsigned char *lo);

// value determines which register to pull when grabbing registers
// (this is for multiple register-based instructions)
char grabRegister(int value, args_t *args);

void createRastislav(rastislav_t *rastislav)
{
    memset(rastislav->regs, 0, RLX + 1);
    memset(rastislav->mem, 0, VIDEO_HI);
    memset(rastislav->ports, 0, PLAYER_2_PORT + 1);
    memcpy(rastislav->rom, rom_const, 1024);

    rastislav->is_on = 1;
    rastislav->is_processing = 1;
    rastislav->is_playing = 0;
}

void destroyRastislav(rastislav_t *rastislav)
{
    rastislav->is_on = 0;
    rastislav->is_processing = 0;
    rastislav->is_playing = 0;
}

unsigned short getRegister(rastislav_t *rastislav, int index)
{
    return rastislav->regs[index];
}

void loadProgram(rastislav_t *rastislav, unsigned char *program)
{
    memcpy(rastislav->mem, program, 16384);
    rastislav->is_playing = 1;
}

void rastislavProcess(rastislav_t *rastislav)
{
    int step, pc, i;
    unsigned char hi, lo;
    unsigned int opcode;
    args_t args;

    step = 2;
    pc = getRegister(rastislav, RPX);

    if(pc >= PROGRAM_HI)
        pc = PROGRAM_LO;

    opcode = opcodeHandshake(rastislav, pc, &hi, &lo);

    for(i = 0; i < NUM_INSTRUCTIONS; ++i)
    {
        instruction_t cur = instructions[i];

        if(cur.hibyte == hi || (cur.hibyte >> 4) == 0x7)
        {
            args = grabArguments(rastislav, &step, pc, hi, lo);
            (*instructions[i].callback)(rastislav, args);
            break;
        }
    }

    // don't step if there is a jump
    if(rastislav->jump == 1)
    {
        step = 0;
        rastislav->jump = 0;
    }

    rastislav->regs[RPX] += step;
}

args_t grabArguments(rastislav_t *rastislav, int *step, int pc, unsigned char hi, unsigned char lo)
{
    args_t args = { 0 };

    instruction_t ins = { 0 };

    unsigned char regA, regB;   // register argument values

    unsigned short value;       // address or real value

    int diff = 0; // the difference between two values (a temp var)
    int temp = 0; // another temp var

    // move operations

    // alu operations
    for(int i = 9; i < (NUM_INSTRUCTIONS - 5); ++i)
    {
        ins = instructions[i];

        if(ins.hibyte == MC_LRV)
            continue;

        diff = (ins.hibyte - ADD); // get the alu operation to perform

        if(ins.hibyte == hi)
        {
            args.alu_operation = diff;
            temp = CHECK_ALU_REAL_VALUE_OP(hi) || CHECK_ALU_ADDRESS_OP(hi);

            if(temp == 1) // check if the 
            {
                regA = (lo & 0x0F);
                (*step) += 2;
                args.regs[regA] = 1;
                value = (rastislav->mem[pc + 2] << 16) | rastislav->mem[pc + 3];

                printf("%d\n%d", regA, rastislav->regs[regA]);

                if(CHECK_ALU_REAL_VALUE_OP(hi))
                    args.value = value;
                else
                    args.address = value;
            } else
            {
                regA = (lo & 0xF0) >> 4;
                regB = (lo & 0x0F);

                if(regA != regB)
                {
                    args.regs[regA] = 1;
                    args.regs[regB] = 2;
                } else
                {
                    args.regs[regA] = 1 | 2;
                }
            }

            return args;
        }
    }

    if(hi == MC_HLT)
        --(*step);
    
    if(hi == MC_LRV)                // LRV
    {
        regA = (hi & 0x0F);
        args.regs[regA] = 1;
        args.value = (lo << 8) | rastislav->mem[pc + 2];
        ++(*step);
    }

    return args;

    // if(hi == WRVP)
    // {
    //     val = (lo << 8) | rastislav->mem[pc + 2];
    //     ++(*step);
    //     args.write_operation = WRVP;
    //     args.value = val;
    //     return args;
    // }

    // // make sure to exclude psh, pop
    // if((n >= 0x4 && n <= 0xA) && n != 0x7 && hi != PSH && hi != POP)    // MOVE
    // {
    //     regA = (lo & 0xF0) >> 4;
    //     regB = (lo & 0x0F);
    //     args.regs[regA] = 1;
    //     args.regs[regB] = 2;
    //     return args;
    // }

    // if(hi == RDP || hi == WTP || hi == PSH || hi == POP) // wtp, rdp, psh, pop
    // {
    //     regA = (lo & 0x0F);
    //     args.regs[regA] = 1;
    //     return args;
    // }

    // if(n == 0x1 || n == 0x2)    // STORE & READ have similar functionality for arguments
    // {
    //     regA = (hi & 0x0F);
    //     addr = (lo << 8) | rastislav->mem[pc + 2];
    //     ++(*step);
    //     args.regs[regA] = 1;
    //     args.address = addr;
    // }

    // if(n > 0xA)                 // ALL JUMP OPERATIONS
    // {
    //     addr = (lo << 8) | rastislav->mem[pc + 2];
    //     ++(*step);
    //     args.address = addr;

    //     args.jmp_operation = n - 0xB;
    // }
}

/*
 - Performs the necessary transaction between the memory and CPU in order to return an opcode.
 - We first pull two bytes of memory relative to the program counter,
 - next we store those values into registers RHX & RLX respectively.
 - Afterwards; we combine the two values into a 16-bit WORD.
*/
unsigned int opcodeHandshake(rastislav_t *rastislav, int pc, unsigned char *hi, unsigned char *lo)
{
    rastislav->regs[RHX] = rastislav->mem[pc];
    
    if(rastislav->regs[RHX] != 0x30)
        rastislav->regs[RLX] = rastislav->mem[pc + 1];

    (*hi) = getRegister(rastislav, RHX);
    (*lo) = getRegister(rastislav, RLX);

    return ((*hi) << 8) | (*lo);
}

char grabRegister(int value, args_t *args)
{
    int i = 0;

    for(i = 0; i < RKX+1; ++i)
        if(args->regs[i] == value)
        {
            args->regs[i] = 0;
            return i; // TODO: remember what the fuck i'mn supposed to be fixing.
        }

    return -1;
}

// CALLBACK FUNCTIONS
void store(rastislav_t *rastislav, args_t args)
{
    unsigned char reg = grabRegister(1, &args);
    unsigned short addr = args.address;

    rastislav->mem[addr] = getRegister(rastislav, reg);
}

void read(rastislav_t *rastislav, args_t args)
{
    unsigned char reg = grabRegister(1, &args);
    unsigned short addr = args.address;

    rastislav->regs[reg] = rastislav->mem[addr];
}

void halt(rastislav_t *rastislav, args_t args)
{
    rastislav->is_playing = 0;
    rastislav->is_processing = 0;
}

void move(rastislav_t *rastislav, args_t args)
{
    unsigned char regA = grabRegister(1, &args);
    unsigned char regB = grabRegister(2, &args);

    rastislav->regs[regB] = rastislav->regs[regA];
}

void determineALUOperands(rastislav_t *rastislav, args_t args, unsigned char *regA, unsigned short *left, unsigned short *right);
short performALUOperation(rastislav_t *rastislav, args_t args, int *flag, unsigned short left, unsigned short right);

void alu(rastislav_t *rastislav, args_t args)
{
    unsigned char regA;
    int flag = 1;
    short result;
    unsigned short left, right;
    left = 0;
    right = 0;

    determineALUOperands(rastislav, args, &regA, &left, &right);
    result = performALUOperation(rastislav, args, &flag, left, right);

    printf("numerator: %d\ndenominator: %d\nresult: %d\n", left, right, result);

    // set the flag register only if the return flag from earlier is > 0. (flag variable has no ties to flag register)
    if(flag > 0)
    {
        if(result == 0)
            rastislav->regs[RFX] = ZERO;
        else
            rastislav->regs[RFX] = NOT | ZERO;

        if(result >= 0xFFFF)
            rastislav->regs[RCX] = CARRY;
        else
            rastislav->regs[RCX] = NOT | CARRY;

        rastislav->regs[regA] = result;
    }
}

void jmp(rastislav_t *rastislav, args_t args)
{
    unsigned short addr = args.address;

    unsigned short carry, flag;
    carry = getRegister(rastislav, RCX);
    flag = getRegister(rastislav, RFX);

    switch(args.jmp_operation)
    {
        case JMP:
            rastislav->regs[RPX] = addr;
        break;
        case JNC:
            if(carry != (NOT | CARRY))
                return;
            
            rastislav->regs[RPX] = addr;
        break;
        case JC:
            if(carry != CARRY)
                return;
            
            rastislav->regs[RPX] = addr;
        break;
        case JNZ:
            if(flag != (NOT | ZERO))
                return;

            rastislav->regs[RPX] = addr;
        break;
        case JZ:
            if(flag != ZERO)
                return;
            
            rastislav->regs[RPX] = addr;
        break;
    }

    rastislav->jump = 1;
}

void lrv(rastislav_t *rastislav, args_t args)
{
    unsigned char reg = grabRegister(1, &args);
    unsigned short value = args.value;

    rastislav->regs[reg] = value;
}

void readPort(rastislav_t *rastislav, args_t args)
{
    unsigned char reg = grabRegister(1, &args);
    unsigned char port = rastislav->regs[RRX]; // get port pointer register value

    rastislav->regs[reg] = rastislav->ports[port];
}

void writePort(rastislav_t *rastislav, args_t args)
{
    // TODO: rewrite to support new instruction (WRVP)

    // unsigned char reg = grabRegister(1, &args);
    // unsigned char port = rastislav->regs[RRX];

    // switch(port)
    // {
    //     case TELETYPE_PORT:
    //         rastislav->regs[RFX] = TTY_OUT;
    //     break;
    //     case AUDIO_PORT:
    //         rastislav->regs[RFX] = AUD_OUT;
    //     break;
    // }

    // rastislav->ports[port] = rastislav->regs[reg];
}

void push(rastislav_t *rastislav, args_t args)
{
    unsigned char reg = grabRegister(1, &args);
    unsigned short stack_ptr = rastislav->regs[RSX];

    if(stack_ptr >= STACK_HI)
    {
        rastislav->is_processing = 0;
        rastislav->is_playing = 0;
        scream("STACK OVERFLOW");
        return;
    }

    printf("PUSHING TO THE STACK\n");

    rastislav->mem[stack_ptr + STACK_LO] = rastislav->regs[reg];
    (rastislav->regs[RSX])++;
}

void pop(rastislav_t *rastislav, args_t args)
{
    unsigned char reg = grabRegister(1, &args);
    unsigned short stack_ptr = rastislav->regs[RSX];

    if(stack_ptr <= 0)
        return;
    
    printf("POPPING FROM THE STACK\n");

    stack_ptr -= 1;
    rastislav->regs[reg] = rastislav->mem[STACK_LO + stack_ptr];
    rastislav->mem[STACK_LO + stack_ptr] = 0;
    (rastislav->regs[RSX])--;
}

void determineALUOperands(rastislav_t *rastislav, args_t args, unsigned char *regA, unsigned short *left, unsigned short *right)
{
    char samereg = 0;

    (*regA) = grabRegister(1, &args);
    if((*regA) < 0)
    {
        (*regA) = grabRegister(3, &args);
        samereg = 1;
    }

    printf("%d\n", regA);

    if(IS_ALU_RV_OP(args.alu_operation))
    {
        (*left) = rastislav->regs[*regA];
        (*right) = args.value;
    } else if(IS_ALU_ADDRESS_OP(args.alu_operation))
    {
        (*left) = rastislav->regs[*regA];
        (*right) = rastislav->mem[args.address];
    } else
    {
        (*left) = rastislav->regs[*regA];

        if(!samereg)
            (*right) = rastislav->regs[grabRegister(2, &args)];
        else
            (*right) = rastislav->regs[*regA];
    }

    printf("left: %d\nright: %d\n", *left, *right);
}

short performALUOperation(rastislav_t *rastislav, args_t args, int *flag, unsigned short left, unsigned short right)
{
    if(args.alu_operation == ADD || args.alu_operation == ARV || args.alu_operation == ADA)
        return (left + right);
    else if(args.alu_operation == MUL || args.alu_operation == MRV || args.alu_operation == MLA)
        return (left * right);
    else if(args.alu_operation == SUB || args.alu_operation == SRV || args.alu_operation == SBA)
        return (left - right);
    else if(args.alu_operation == DIV || args.alu_operation == DRV || args.alu_operation == DVA)
        return (left / right);
    else if(args.alu_operation == XOR || args.alu_operation == XRV || args.alu_operation == XRA)
        return (left ^ right);
    else if(args.alu_operation == OR  || args.alu_operation == ORV || args.alu_operation == ORA)
        return (left | right);
    else if(args.alu_operation == AND || args.alu_operation == ANDV || args.alu_operation == ANDA)
        return (left & right);

    if(args.alu_operation == CMPR || args.alu_operation == CMPV || args.alu_operation == CMPA)
    {
        if(left == right)
            rastislav->regs[RFX] = EQUAL;
        else if(left != right)
            rastislav->regs[RFX] = NOT | EQUAL;
        else if(left >= right)
            rastislav->regs[RFX] = GREATER_EQUAL;
        else if(left > right)
            rastislav->regs[RFX] = GREATER;
        else if(left <= right)
            rastislav->regs[RFX] = LESSER;
        else if(left < right)
            rastislav->regs[RFX] = LESSER_EQUAL;
        
        (*flag) = 1;
    }

    return 0;
}