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

  },
  [INSTR_JMP] = {

  },
  [INSTR_BEQ] = {

  },
  [INSTR_BNE] = {

  },
  [INSTR_BGT] = {

  },
  [INSTR_PUSH] = {

  },
  [INSTR_POP] = {

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

  },
  [INSTR_STR] = {

  },
  [INSTR_CSRRD] = {

  },
  [INSTR_CSRWR] = {

  },
  
};