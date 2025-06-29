#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdlib.h>

typedef unsigned char Byte;

typedef struct Processor{
    Byte a, sp, x , pc, ir;
    bool z , p, c;
} Processor;
typedef enum {
    EMU_STATUS_RUNNING,
    EMU_STATUS_FINISHED,
    EMU_STATUS_NO_DATA,
    EMU_STATUS_BAD_DATA,
    EMU_STATUS_BAD_OP,
    EMU_STATUS_COUNT
}Status;

typedef struct Emulator{
    Processor cpu;
    Byte mem[4294967296];
    Status status;
}Emulator;

Emulator* emulatorCreate();
void emulatorLoad(Emulator* emulator,File* inputFile);

#endif