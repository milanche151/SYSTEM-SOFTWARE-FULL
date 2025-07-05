#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef unsigned char Byte;

typedef struct Processor{
    uint32_t reg[16];
    bool z, p, c;
} Processor;
typedef enum {
    EMU_STATUS_RUNNING,
    EMU_STATUS_FINISHED,
    EMU_STATUS_NO_DATA,
    EMU_STATUS_BAD_DATA,
    EMU_STATUS_BAD_OP,
    EMU_STATUS_COUNT
}Status;

// Enum za procesorske instrukcije na osnovu opcode-a (prvi bajt instrukcije)
typedef enum {
    INSTR_HALT = 0x00,        // 0000 0000 - Instrukcija za zaustavljanje procesora
    INSTR_INT = 0x10,         // 0001 0000 - Instrukcija softverskog prekida
    INSTR_CALL = 0x20,        // 0010 MMMM - Instrukcija poziva potprograma
    INSTR_JMP = 0x30,         // 0011 MMMM - Instrukcija skoka
    INSTR_XCHG = 0x40,        // 0100 0000 - Instrukcija atomične zamene vrednosti
    INSTR_ARITH = 0x50,       // 0101 MMMM - Instrukcija aritmetičkih operacija
    INSTR_LOGIC = 0x60,       // 0110 MMMM - Instrukcija logičkih operacija
    INSTR_SHIFT = 0x70,       // 0111 MMMM - Instrukcija pomeračkih operacija
    INSTR_STORE = 0x80,       // 1000 MMMM - Instrukcija smeštanja podatka
    INSTR_LOAD = 0x90         // 1001 MMMM - Instrukcija učitavanja podatka
} InstructionOpcode;

// Modifikatori za instrukcije poziva potprograma (CALL)
typedef enum {
    CALL_REG_INDIRECT = 0x0,    // push pc; pc<=gpr[A]+gpr[B]+D;
    CALL_MEM_INDIRECT = 0x1     // push pc; pc<=mem32[gpr[A]+gpr[B]+D];
} CallModifier;

// Modifikatori za instrukcije skoka (JMP)
typedef enum {
    JMP_UNCONDITIONAL = 0x0,    // pc<=gpr[A]+D;
    JMP_EQ = 0x1,              // if (gpr[B] == gpr[C]) pc<=gpr[A]+D;
    JMP_NE = 0x2,              // if (gpr[B] != gpr[C]) pc<=gpr[A]+D;
    JMP_GT = 0x3,              // if (gpr[B] signed> gpr[C]) pc<=gpr[A]+D;
    JMP_MEM_UNCONDITIONAL = 0x8, // pc<=mem32[gpr[A]+D];
    JMP_MEM_EQ = 0x9,          // if (gpr[B] == gpr[C]) pc<=mem32[gpr[A]+D];
    JMP_MEM_NE = 0xA,          // if (gpr[B] != gpr[C]) pc<=mem32[gpr[A]+D];
    JMP_MEM_GT = 0xB           // if (gpr[B] signed> gpr[C]) pc<=mem32[gpr[A]+D];
} JumpModifier;

// Modifikatori za aritmetičke operacije
typedef enum {
    ARITH_ADD = 0x0,           // gpr[A]<=gpr[B] + gpr[C];
    ARITH_SUB = 0x1,           // gpr[A]<=gpr[B] - gpr[C];
    ARITH_MUL = 0x2,           // gpr[A]<=gpr[B] * gpr[C];
    ARITH_DIV = 0x3            // gpr[A]<=gpr[B] / gpr[C];
} ArithModifier;

// Modifikatori za logičke operacije
typedef enum {
    LOGIC_NOT = 0x0,           // gpr[A]<=~gpr[B];
    LOGIC_AND = 0x1,           // gpr[A]<=gpr[B] & gpr[C];
    LOGIC_OR = 0x2,            // gpr[A]<=gpr[B] | gpr[C];
    LOGIC_XOR = 0x3            // gpr[A]<=gpr[B] ^ gpr[C];
} LogicModifier;

// Modifikatori za pomeranje
typedef enum {
    SHIFT_LEFT = 0x0,          // gpr[A]<=gpr[B] << gpr[C];
    SHIFT_RIGHT = 0x1          // gpr[A]<=gpr[B] >> gpr[C];
} ShiftModifier;

// Modifikatori za store instrukcije
typedef enum {
    STORE_REG_INDIRECT = 0x0,   // mem32[gpr[A]+gpr[B]+D]<=gpr[C];
    STORE_REG_INDIRECT_POST = 0x1, // gpr[A]<=gpr[A]+D; mem32[gpr[A]]<=gpr[C];
    STORE_MEM_INDIRECT = 0x2    // mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];
} StoreModifier;

// Modifikatori za load instrukcije
typedef enum {
    LOAD_CSR = 0x0,            // gpr[A]<=csr[B];
    LOAD_REG_DISP = 0x1,       // gpr[A]<=gpr[B]+D;
    LOAD_MEM_INDIRECT = 0x2,   // gpr[A]<=mem32[gpr[B]+gpr[C]+D];
    LOAD_MEM_INDIRECT_POST = 0x3, // gpr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
    LOAD_CSR_FROM_REG = 0x4,   // csr[A]<=gpr[B];
    LOAD_CSR_OR_DISP = 0x5,    // csr[A]<=csr[B]|D;
    LOAD_CSR_MEM_INDIRECT = 0x6, // csr[A]<=mem32[gpr[B]+gpr[C]+D];
    LOAD_CSR_MEM_POST = 0x7    // csr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
} LoadModifier;

#define FRAME_SIZE 0x1000
#define PMT1_SIZE 0x100
#define PMT2_SIZE 0x100

typedef char MemFrame [FRAME_SIZE];
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
void emulatorMemoryTest(Emulator *emulator);

// Pomoćne funkcije za dekodiranje instrukcija
InstructionOpcode getInstructionOpcode(Byte instruction);
Byte getInstructionModifier(Byte instruction);

#endif