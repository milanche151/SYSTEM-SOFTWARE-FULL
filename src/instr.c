#include "instr.h"

InstrDesc instr_descs[INSTR_TYPE_COUNT] = {
  [INSTR_HALT] = {
    .family = INSTR_FAMILY_NOOP,
    .opcode = 0x00,
    .modifier = 0x00,
    .name = "halt",
  },
  [INSTR_INT] = {

  },
  [INSTR_IRET] = {

  },
  [INSTR_RET] = {

  },
  [INSTR_CALL] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x00,
    .name = "call",
  },
  [INSTR_JMP] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x00,
    .name = "jmp",
  },
  [INSTR_BEQ] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x01,
    .name = "beq",
  },
  [INSTR_BNE] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x02,
    .name = "bne",
  },
  [INSTR_BGT] = {
    .family = INSTR_FAMILY_TWOREG_ONEOP,
    .opcode = 0x03,
    .modifier = 0x03,
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
  [INSTR_NOT] = {

  },
  [INSTR_XCHG] = {

  },
  [INSTR_ADD] = {
    .family = INSTR_FAMILY_TWOREG,
    .opcode = 0x5,
    .modifier = 0x0,
    .name = "add",
  },
  [INSTR_SUB] = {

  },
  [INSTR_MUL] = {

  },
  [INSTR_DIV] = {

  },
  [INSTR_AND] = {

  },
  [INSTR_OR] = {

  },
  [INSTR_XOR] = {

  },
  [INSTR_SHL] = {

  },
  [INSTR_SHR] = {

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
    .name = "st"
  },
  [INSTR_CSRRD] = {

  },
  [INSTR_CSRWR] = {

  },
  
};