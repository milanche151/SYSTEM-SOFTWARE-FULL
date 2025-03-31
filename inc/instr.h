#ifndef _INSTR_H_
#define _INSTR_H_

typedef enum {
  INSTR_HALT,
  INSTR_INT,
  INSTR_IRET,
  INSTR_RET,

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

typedef struct InstrDesc {
  InstrFamily family;
  unsigned char opcode;
  unsigned char modifier;
  const char *name;
} InstrDesc;

extern InstrDesc instr_descs[INSTR_TYPE_COUNT];

typedef struct Instruction
{
  InstrType type;
  int reg1;
  int reg2;
} Instruction;

#endif