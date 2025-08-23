#include "instr.h"

InstrDesc instr_descs[INSTR_TYPE_COUNT] = {
  [INSTR_HALT] = {
    .family = INSTR_FAMILY_NOOP,
    .opcode = 0x00,
    .modifier = 0x00,
    .name = "halt",
  },
  [INSTR_INT] = {
    .family = INSTR_FAMILY_NOOP,
    .opcode = 0x01,
    .modifier = 0x00,
    .name = "int",
  },
  [INSTR_IRET] = {
    .family = INSTR_FAMILY_SPECIAL,
    .name = "iret",
  },
  [INSTR_RET] = {
    .family = INSTR_FAMILY_SPECIAL,
    .name = "ret",
  },
  [INSTR_CALL] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x02,
    .modifier = 0x01,
    .name = "call",
  },
  [INSTR_JMP] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x08,
    .name = "jmp",
  },
  [INSTR_BEQ] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x09,
    .name = "beq",
  },
  [INSTR_BNE] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x0a,
    .name = "bne",
  },
  [INSTR_BGT] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x0b,
    .name = "bgt",
  },
  [INSTR_PUSH] = {
    .family = INSTR_FAMILY_ONEREG,
    .opcode = 0x08,
    .modifier = 0x01,
    .name = "push",
  },
  [INSTR_POP] = {
    .family = INSTR_FAMILY_ONEREG,
    .opcode = 0x09,
    .modifier = 0x03,
    .name = "pop",
  },
  [INSTR_XCHG] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x4,
    .modifier = 0x0,
    .name = "xchg",
  },
  [INSTR_ADD] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x5,
    .modifier = 0x0,
    .name = "add",
  },
  [INSTR_SUB] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x5,
    .modifier = 0x1,
    .name = "sub",
  },
  [INSTR_MUL] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x5,
    .modifier = 0x2,
    .name = "mul",
  },
  [INSTR_DIV] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x5,
    .modifier = 0x3,
    .name = "div",
  },
  [INSTR_MOD] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x5,
    .modifier = 0x4,
    .name = "mod",
  },
  [INSTR_NOT] = {
    .family = INSTR_FAMILY_ONEREG,
    .opcode = 0x6,
    .modifier = 0x0,
    .name = "not",
  },
  [INSTR_AND] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x6,
    .modifier = 0x1,
    .name = "and",
  },
  [INSTR_OR] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x6,
    .modifier = 0x2,
    .name = "or",
  },
  [INSTR_XOR] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x6,
    .modifier = 0x3,
    .name = "xor",
  },
  [INSTR_SHL] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x7,
    .modifier = 0x0,
    .name = "shl",
  },
  [INSTR_SHR] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x7,
    .modifier = 0x1,
    .name = "shr",
  },
  [INSTR_LD] = {
    .family = INSTR_FAMILY_LD,
    .opcode = 0x09,
    .modifier = 0x00,
    .name = "ld"
  },
  [INSTR_STR] = {
    .family = INSTR_FAMILY_STR,
    .opcode = 0x08,
    .modifier = 0x00,
    .name = "str"
  },
  [INSTR_CSRRD] = {
    .family = INSTR_FAMILY_CSRRD,
    .opcode = 0x09,
    .modifier = 0x00,
    .name = "csrrd"
  },
  [INSTR_CSRWR] = {
    .family = INSTR_FAMILY_CSRWR,
    .opcode = 0x09,
    .modifier = 0x04,
    .name = "csrwr"
  },
  
};