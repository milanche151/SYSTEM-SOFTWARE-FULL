#ifndef _INSTR_H_
#define _INSTR_H_

typedef enum {
  INSTR_HALT,
  INSTR_INT,
  INSTR_RET,
  INSTR_IRET,

  INSTR_CALL,
  INSTR_JMP,

  INSTR_BEQ,
  INSTR_BNE,
  INSTR_BGT,

  INSTR_PUSH,
  INSTR_POP,
  INSTR_NOT,

  INSTR_XCHG,
  INSTR_ADD,
  INSTR_SUB,
  INSTR_MUL,
  INSTR_DIV,
  INSTR_MOD,
  INSTR_AND,
  INSTR_OR,
  INSTR_XOR,
  INSTR_SHL,
  INSTR_SHR,

  INSTR_LD,
  INSTR_STR,
  INSTR_CSRRD,
  INSTR_CSRWR,

  INSTR_TYPE_COUNT,

} InstrType;

typedef enum {
  INSTR_FAMILY_ERROR, // this value should never be assigned

  INSTR_FAMILY_SPECIAL,
  INSTR_FAMILY_NOOP,
  INSTR_FAMILY_ONEOP,
  INSTR_FAMILY_TWOREG_ONEOP,
  INSTR_FAMILY_ONEREG,
  INSTR_FAMILY_TWOREG,
  INSTR_FAMILY_LD,
  INSTR_FAMILY_STR,
  INSTR_FAMILY_CSRRD,
  INSTR_FAMILY_CSRWR,

  INSTR_FAMILY_COUNT,
} InstrFamily;

typedef enum OperandType{
  OPERAND_TYPE_IMMED_LIT,
  OPERAND_TYPE_IMMED_SYM,
  OPERAND_TYPE_MEMDIR_LIT,
  OPERAND_TYPE_MEMDIR_SYM,
  OPERAND_TYPE_REGDIR,
  OPERAND_TYPE_REGIND,
  OPERAND_TYPE_REGIND_LIT,
  OPERAND_TYPE_REGIND_SYM,

  OPERAND_TYPE_COUNT,
}OperandType;
typedef struct Operand{
  OperandType type;
  int reg;
  int literal;
  const char* symbol;
}Operand;
// Instruction type (1st byte)
typedef enum {
    ASM_INSTR_HALT = 0x0,        // 0000 0000  
    ASM_INSTR_INT = 0x1,         // 0001 0000 
    ASM_INSTR_CALL = 0x2,        // 0010 MMMM  
    ASM_INSTR_JMP = 0x3,         // 0011 MMMM  
    ASM_INSTR_XCHG = 0x4,        // 0100 0000 - temp<=gpr[B]; gpr[B]<=gpr[C]; gpr[C]<=temp;
    ASM_INSTR_ARITH = 0x5,       // 0101 MMMM  
    ASM_INSTR_LOGIC = 0x6,       // 0110 MMMM  
    ASM_INSTR_SHIFT = 0x7,       // 0111 MMMM  
    ASM_INSTR_STORE = 0x8,       // 1000 MMMM  
    ASM_INSTR_LOAD = 0x9,         // 1001 MMMM  
    ASM_INSTR_COUNT
} InstructionOpcode;

typedef struct Instruction
{
  InstrType type;
  InstrFamily family;
  int reg1;
  int reg2;
  Operand operand;
} Instruction;
// Modifiers call operations (CALL)
typedef enum {
    ASM_CALL_REG_INDIRECT = 0x0,    // push pc; pc<=gpr[A]+gpr[B]+D;
    ASM_CALL_MEM_INDIRECT = 0x1     // push pc; pc<=mem32[gpr[A]+gpr[B]+D];
} CallModifier;

// Modifiers for jump operations (JMP)
typedef enum {
    ASM_JMP_UNCONDITIONAL = 0x0,    // pc<=gpr[A]+D;
    ASM_JMP_EQ = 0x1,              // if (gpr[B] == gpr[C]) pc<=gpr[A]+D;
    ASM_JMP_NE = 0x2,              // if (gpr[B] != gpr[C]) pc<=gpr[A]+D;
    ASM_JMP_GT = 0x3,              // if (gpr[B] signed> gpr[C]) pc<=gpr[A]+D;
    ASM_JMP_MEM_UNCONDITIONAL = 0x8, // pc<=mem32[gpr[A]+D];
    ASM_JMP_MEM_EQ = 0x9,          // if (gpr[B] == gpr[C]) pc<=mem32[gpr[A]+D];
    ASM_JMP_MEM_NE = 0xA,          // if (gpr[B] != gpr[C]) pc<=mem32[gpr[A]+D];
    ASM_JMP_MEM_GT = 0xB,           // if (gpr[B] signed> gpr[C]) pc<=mem32[gpr[A]+D];
} JumpModifier;

// Modifiers for arithmetic operations
typedef enum {
    ASM_ARITH_ADD = 0x0,           // gpr[A]<=gpr[B] + gpr[C];
    ASM_ARITH_SUB = 0x1,           // gpr[A]<=gpr[B] - gpr[C];
    ASM_ARITH_MUL = 0x2,           // gpr[A]<=gpr[B] * gpr[C];
    ASM_ARITH_DIV = 0x3,           // gpr[A]<=gpr[B] / gpr[C];
    ASM_ARITH_MOD = 0x4,           // gpr[A]<=gpr[B] % gpr[C];
} ArithModifier;

// Modifiers for logic operations
typedef enum {
    ASM_LOGIC_NOT = 0x0,           // gpr[A]<=~gpr[B];
    ASM_LOGIC_AND = 0x1,           // gpr[A]<=gpr[B] & gpr[C];
    ASM_LOGIC_OR = 0x2,            // gpr[A]<=gpr[B] | gpr[C];
    ASM_LOGIC_XOR = 0x3            // gpr[A]<=gpr[B] ^ gpr[C];
} LogicModifier;

// Modifiers for shift operations
typedef enum {
    ASM_SHIFT_LEFT = 0x0,          // gpr[A]<=gpr[B] << gpr[C];
    ASM_SHIFT_RIGHT = 0x1          // gpr[A]<=gpr[B] >> gpr[C];
} ShiftModifier;

// Modifiers for store operations
typedef enum {
    ASM_STORE_REG_INDIRECT = 0x0,   // mem32[gpr[A]+gpr[B]+D]<=gpr[C];
    ASM_STORE_REG_INDIRECT_PREINC = 0x1, // gpr[A]<=gpr[A]+D; mem32[gpr[A]]<=gpr[C];
    ASM_STORE_MEM_INDIRECT = 0x2    // mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];
} StoreModifier;

// Modifiers for load operations
typedef enum {
    ASM_LOAD_CSR = 0x0,            // gpr[A]<=csr[B];
    ASM_LOAD_REG_DISP = 0x1,       // gpr[A]<=gpr[B]+D;
    ASM_LOAD_MEM_INDIRECT = 0x2,   // gpr[A]<=mem32[gpr[B]+gpr[C]+D];
    ASM_LOAD_MEM_INDIRECT_POSTINC = 0x3, // gpr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
    ASM_LOAD_CSR_FROM_REG = 0x4,   // csr[A]<=gpr[B];
    ASM_LOAD_CSR_OR_DISP = 0x5,    // csr[A]<=csr[B]|D;
    ASM_LOAD_CSR_MEM_INDIRECT = 0x6, // csr[A]<=mem32[gpr[B]+gpr[C]+D];
    ASM_LOAD_CSR_MEM_POSTINC = 0x7    // csr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
} LoadModifier;

#endif