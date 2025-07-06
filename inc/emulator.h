#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned char Byte;

typedef struct Processor{
    uint32_t reg[16];
    uint32_t csr[3];
} Processor;
typedef enum {
    EMU_STATUS_LOAD_FAILED,
    EMU_STATUS_RUNNING,
    EMU_STATUS_FINISHED,
    EMU_STATUS_BUS_ERROR, /* when accessing misaligned memory */
    EMU_STATUS_BAD_OP,
    EMU_STATUS_BAD_MOD,
    EMU_STATUS_DIV_BY_ZERO, 
    EMU_STATUS_COUNT
}Status;

// Instruction type (1st byte)
typedef enum {
    INSTR_HALT = 0x0,        // 0000 0000  
    INSTR_INT = 0x1,         // 0001 0000 
    INSTR_CALL = 0x2,        // 0010 MMMM  
    INSTR_JMP = 0x3,         // 0011 MMMM  
    INSTR_XCHG = 0x4,        // 0100 0000 - temp<=gpr[B]; gpr[B]<=gpr[C]; gpr[C]<=temp;
    INSTR_ARITH = 0x5,       // 0101 MMMM  
    INSTR_LOGIC = 0x6,       // 0110 MMMM  
    INSTR_SHIFT = 0x7,       // 0111 MMMM  
    INSTR_STORE = 0x8,       // 1000 MMMM  
    INSTR_LOAD = 0x9,         // 1001 MMMM  
    INSTR_COUNT
} InstructionOpcode;

// Modifiers call operations (CALL)
typedef enum {
    CALL_REG_INDIRECT = 0x0,    // push pc; pc<=gpr[A]+gpr[B]+D;
    CALL_MEM_INDIRECT = 0x1     // push pc; pc<=mem32[gpr[A]+gpr[B]+D];
} CallModifier;

// Modifiers for jump operations (JMP)
typedef enum {
    JMP_UNCONDITIONAL = 0x0,    // pc<=gpr[A]+D;
    JMP_EQ = 0x1,              // if (gpr[B] == gpr[C]) pc<=gpr[A]+D;
    JMP_NE = 0x2,              // if (gpr[B] != gpr[C]) pc<=gpr[A]+D;
    JMP_GT = 0x3,              // if (gpr[B] signed> gpr[C]) pc<=gpr[A]+D;
    JMP_MEM_UNCONDITIONAL = 0x8, // pc<=mem32[gpr[A]+D];
    JMP_MEM_EQ = 0x9,          // if (gpr[B] == gpr[C]) pc<=mem32[gpr[A]+D];
    JMP_MEM_NE = 0xA,          // if (gpr[B] != gpr[C]) pc<=mem32[gpr[A]+D];
    JMP_MEM_GT = 0xB,           // if (gpr[B] signed> gpr[C]) pc<=mem32[gpr[A]+D];
} JumpModifier;

// Modifiers for arithmetic operations
typedef enum {
    ARITH_ADD = 0x0,           // gpr[A]<=gpr[B] + gpr[C];
    ARITH_SUB = 0x1,           // gpr[A]<=gpr[B] - gpr[C];
    ARITH_MUL = 0x2,           // gpr[A]<=gpr[B] * gpr[C];
    ARITH_DIV = 0x3            // gpr[A]<=gpr[B] / gpr[C];
} ArithModifier;

// Modifiers for logic operations
typedef enum {
    LOGIC_NOT = 0x0,           // gpr[A]<=~gpr[B];
    LOGIC_AND = 0x1,           // gpr[A]<=gpr[B] & gpr[C];
    LOGIC_OR = 0x2,            // gpr[A]<=gpr[B] | gpr[C];
    LOGIC_XOR = 0x3            // gpr[A]<=gpr[B] ^ gpr[C];
} LogicModifier;

// Modifiers for shift operations
typedef enum {
    SHIFT_LEFT = 0x0,          // gpr[A]<=gpr[B] << gpr[C];
    SHIFT_RIGHT = 0x1          // gpr[A]<=gpr[B] >> gpr[C];
} ShiftModifier;

// Modifiers for store operations
typedef enum {
    STORE_REG_INDIRECT = 0x0,   // mem32[gpr[A]+gpr[B]+D]<=gpr[C];
    STORE_REG_INDIRECT_PREINC = 0x1, // gpr[A]<=gpr[A]+D; mem32[gpr[A]]<=gpr[C];
    STORE_MEM_INDIRECT = 0x2    // mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];
} StoreModifier;

// Modifiers for load operations
typedef enum {
    LOAD_CSR = 0x0,            // gpr[A]<=csr[B];
    LOAD_REG_DISP = 0x1,       // gpr[A]<=gpr[B]+D;
    LOAD_MEM_INDIRECT = 0x2,   // gpr[A]<=mem32[gpr[B]+gpr[C]+D];
    LOAD_MEM_INDIRECT_POSTINC = 0x3, // gpr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
    LOAD_CSR_FROM_REG = 0x4,   // csr[A]<=gpr[B];
    LOAD_CSR_OR_DISP = 0x5,    // csr[A]<=csr[B]|D;
    LOAD_CSR_MEM_INDIRECT = 0x6, // csr[A]<=mem32[gpr[B]+gpr[C]+D];
    LOAD_CSR_MEM_POSTINC = 0x7    // csr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
} LoadModifier;

#define FRAME_SIZE 0x1000
#define PMT1_SIZE 0x100
#define PMT2_SIZE 0x100

typedef Byte MemFrame [FRAME_SIZE];
typedef MemFrame* pmt1[PMT1_SIZE];
typedef pmt1* pmt2[PMT2_SIZE];

typedef struct Emulator{
    Processor cpu;
    pmt2 mem;
    Status status;
}Emulator;

Emulator emulatorCreate();
void emulatorDestroy(Emulator *emu);
void emulatorLoad(Emulator* emulator, FILE* inputFile);
void emulatorRun(Emulator* emulator);
void emulatorMemoryTest(Emulator *emulator);

// Pomoćne funkcije za dekodiranje instrukcija
InstructionOpcode getInstructionOpcode(Byte instruction);
Byte getInstructionModifier(Byte instruction);

#endif