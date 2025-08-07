#include "emulator.h"
#include "util.h"
#include <assert.h>

#define START_PC 0x40000000
#define REG_PC 15
#define REG_SP 14
#define TIM_CFG_ADDR 0xFFFFFF10

typedef enum csr_reg {
  CSR_STATUS,
  CSR_HANDLER,
  CSR_CAUSE,

  CSR_COUNT,
} csr_reg;



Emulator emulatorCreate(){
  Emulator em = { 0 };
  em.cpu.reg[REG_PC] = START_PC;
  em.timer = (Timer){
    .curr_time = 0,
    .set_time = 0,
    .start_time = 0
  };
  return em;
}

void emulatorDestroy(Emulator *emu){
  for(size_t i = 0; i < PMT2_SIZE; i++){
    if(emu->mem[i] != NULL){
      for(size_t j = 0; j < PMT1_SIZE; j++){
        if((*emu->mem[i])[j] != NULL){
          myFree((*emu->mem[i])[j]);
        }
      }
      myFree(emu->mem[i]);
    }
  }
}

static Byte*  getMemoryAddr(Emulator* emu, uint32_t addr){
  size_t pmt2_offset = addr>>24;
  size_t pmt1_offset = (addr<<8)>>24;
  size_t frame_offset = (addr<<16)>>16;

  if(emu->mem[pmt2_offset]==NULL){
    emu->mem[pmt2_offset] = myMalloc(sizeof(*emu->mem[pmt2_offset]));
    for(size_t i = 0; i < PMT1_SIZE; i++){
      (*emu->mem[pmt2_offset])[i] = NULL;
    }
  }
  pmt1* curr_pmt1 = emu->mem[pmt2_offset];
  
  if((*curr_pmt1)[pmt1_offset] == NULL){
    (*curr_pmt1)[pmt1_offset] = myMalloc(sizeof(*(*curr_pmt1)[pmt1_offset]));
    for (size_t i = 0; i < FRAME_SIZE; i++){
      (*(*curr_pmt1)[pmt1_offset])[i] = 0;
    }
  }
  MemFrame* curr_memframe = (*curr_pmt1)[pmt1_offset];

  return &(*curr_memframe)[frame_offset];
}

static uint32_t memoryReadWord(Emulator *emu, uint32_t addr, bool* isAligned){
  *isAligned = *isAligned && addr % 4 == 0;
  if(!*isAligned) return 0;  /* don't care about the value */

  uint32_t word = 0;
  for (size_t i = 4; i-->0 ;){
    word = word<<8 | *getMemoryAddr(emu,addr+i);
  }
  return word;
}

static uint32_t memoryReadWordBigEndian(Emulator *emu, uint32_t addr, bool* isAligned){
  *isAligned = *isAligned && addr % 4 == 0;
  if(!*isAligned) return 0;  /* don't care about the value */

  uint32_t word = 0;
  for (size_t i = 0; i < 4; i++){
    word = word<<8 | *getMemoryAddr(emu,addr+i);
  }
  return word;
}

static void memoryWriteWord(Emulator *emu, uint32_t addr, uint32_t word,bool* isAligned){
  *isAligned = *isAligned && addr % 4 == 0;
  if(!*isAligned) return;

  if(addr == TIM_CFG_ADDR && word < 0x8){
    emu->timer.set_time = (word + 1)*500; 
  }

  for (size_t i = 0; i < 4; i++){
    *getMemoryAddr(emu,addr+i) = word;
    word >>= 8;
  }
  
}
void tick(Emulator* emu){
  emu->timer.curr_time=clock();
  double timedif = (double)(1000*(emu->timer.curr_time - emu->timer.start_time))/ CLOCKS_PER_SEC;
  //printf("Timer : %lu\n", timedif);
  if(timedif >= emu->timer.set_time){
    emu->status = EMU_STATUS_TIMER_INTERRUPT;
    emu->timer.start_time = emu->timer.curr_time;
  }
}

void emulatorLoad(Emulator* emu, FILE* inputFile){
  uint32_t addr;
  uint32_t byte;

  printf("Emulator loading:\n");
  while(true){
    int read = 0;
    read = fscanf(inputFile,"%x:",&addr);
    if(read != 1) {
      printf("Hex file loaded successfully.\n");
      emu->status = EMU_STATUS_RUNNING;
      return;
    };

    printf("%08x: ", addr);

    for (size_t i = 0; i < 8; i++){
      read = fscanf(inputFile,"%x",&byte);
      if(read != 1) break;

      *getMemoryAddr(emu,addr+i) = (Byte)byte;

      printf("%02x ", byte);
    }

    printf("\n");
  }

  printf("Incorrect input hex file.\n");
  emu->status = EMU_STATUS_LOAD_FAILED;
  return;

}

#define REG(x) (emu->cpu.reg[(x)])
#define CSR(x) (emu->cpu.csr[(x)])
#define ADVANCE_PC() ((void)(REG(REG_PC) += 4))

static void emulatorPrintFrame(uint32_t base_addr, const MemFrame *memframe){
  assert(base_addr % 0x10000 == 0);

  for(size_t i = 0; i < FRAME_SIZE; i += 16){
    bool is_used = false;
    
    for(size_t j = 0; j < 16; j++){
      if((*memframe)[i + j] != 0) {
        is_used = true;
        break;
      }
    }

    if(is_used){
      printf("%08x: ", (uint32_t)(base_addr + i));
      for(size_t j = 0; j < 16; j++){
        printf("%02x ", (*memframe)[i + j]);
      }
      printf("\n");
    }
  }
}

static void emulatorPrint(const Emulator* emu){
  printf("\n");
  printf("Memory content:\n");
  for(size_t i = 0; i < PMT2_SIZE; i++){
    pmt1 *curr_pmt1 = emu->mem[i];
    if(curr_pmt1 == NULL) continue;
    
    for(size_t j = 0; j < PMT1_SIZE; j++){
      MemFrame *curr_frame = (*curr_pmt1)[j];
      if(curr_frame == NULL) continue;

      emulatorPrintFrame((i * 0x100 + j) * 0x10000, curr_frame);
    }
  }
  printf("\n");

  printf("Processor registers:\n");
  for (size_t i = 0; i < 16; i++){
    printf("r%-2lu = %08x\n",i,emu->cpu.reg[i]);
  }
  printf("\n");
  printf("Timer Period:%ld miliseconds\n",emu->timer.set_time);
}

static void emulatorRaiseExpection(Emulator *emu, int cause){
  // can't handle exception if already inside interrupt routine
  if(CSR(CSR_STATUS) & STATUS_INTERRUPT_BIT != 0) {
    return;
  }

  assert(cause >= 0 && cause <= 4);

  bool isAligned = true;

  REG(REG_SP) -= 4;
  memoryWriteWord(emu, REG(REG_SP), CSR(CSR_STATUS), &isAligned);
  REG(REG_SP) -= 4;
  memoryWriteWord(emu, REG(REG_SP), REG(REG_PC), &isAligned);

  if(isAligned){
    emu->status = EMU_STATUS_RUNNING;

    CSR(CSR_STATUS) |= 0x07;
    CSR(CSR_CAUSE) = cause;
    REG(REG_PC) = CSR(CSR_HANDLER);
  }
}

void emulatorRun(Emulator* emu){
  bool isAligned = true;
  emu->timer.set_time = 500;
  emu->timer.start_time = clock();
  emu->timer.curr_time = emu->timer.start_time;
  memoryWriteWord(emu,TIM_CFG_ADDR,0,&isAligned);
  while(emu->status == EMU_STATUS_RUNNING){
    if(REG(REG_PC) % 4 != 0) {
      emu->status == EMU_STATUS_BUS_ERROR;
      continue;
    }

    uint32_t instr = memoryReadWordBigEndian(emu, REG(REG_PC),&isAligned);
    uint32_t opcode = (instr << 0) >> 28;
    uint32_t mod    = (instr << 4) >> 28;
    uint32_t a      = (instr << 8) >> 28;
    uint32_t b      = (instr << 12) >> 28;
    uint32_t c      = (instr << 16) >> 28;
    uint32_t d      = ((int32_t)instr << 20) >> 20;

    ADVANCE_PC();
    tick(emu);
    
    assert(opcode < 0x10 && mod < 0x10 && a < 0x10 && b < 0x10 && c < 0x10
      && (d >= -0x800 || d < 0x800));
    if(emu->status == EMU_STATUS_RUNNING){
      switch ((InstructionOpcode)opcode)
      {
      case INSTR_HALT:
        emu->status = EMU_STATUS_FINISHED;
        break;

      case INSTR_INT:
        emu->status = EMU_STATUS_SOFTWARE_INTERRUPT;
        break;

      case INSTR_CALL:{
        switch ((CallModifier)mod)
        {
        case CALL_REG_INDIRECT:
          REG(REG_SP) -= 4;
          memoryWriteWord(emu, REG(REG_SP), REG_PC, &isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          REG(REG_PC) = REG(a)+REG(b)+d;
          break;
        case CALL_MEM_INDIRECT:
          REG(REG_SP) -= 4;
          memoryWriteWord(emu, REG(REG_SP), REG_PC, &isAligned);
          REG(REG_PC) = memoryReadWord(emu,REG(a)+REG(b)+d,&isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          break;
        default:
          emu->status = EMU_STATUS_BAD_MOD;
          break;
        }
      } break;

      case INSTR_JMP:{
        switch ((JumpModifier)mod)
        {
        case JMP_UNCONDITIONAL:
          REG(REG_PC) = REG(a)+d;
          break;
        case JMP_EQ:
          if(REG(b)== REG(c)) REG(REG_PC) = REG(a)+d;
          break;
        case JMP_NE:
          if(REG(b)!= REG(c)) REG(REG_PC) = REG(a)+d;
          break;
        case JMP_GT:
          if((int32_t)REG(b) > (int32_t)REG(c)) REG(REG_PC) = REG(a)+d;
          break;
        case JMP_MEM_UNCONDITIONAL:
          REG(REG_PC) = memoryReadWord(emu,REG(a)+d,&isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case JMP_MEM_EQ:
          if(REG(b)== REG(c)) {
            REG(REG_PC) = memoryReadWord(emu,REG(a)+d,&isAligned);
            if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          }
          break;
        case JMP_MEM_NE:
          if(REG(b)!= REG(c)){
            REG(REG_PC) = memoryReadWord(emu,REG(a)+d,&isAligned);
            if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          }
          break;
        case JMP_MEM_GT:
          if((int32_t)REG(b) > (int32_t)REG(c)){
            REG(REG_PC) = memoryReadWord(emu,REG(a)+d,&isAligned);
            if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          }
          break;
        default:
          emu->status = EMU_STATUS_BAD_MOD;
          break;
        }
      } break;

      case INSTR_XCHG:{
        REG(b) = REG(c)^REG(b);
        REG(c) = REG(c)^REG(b);
        REG(b) = REG(c)^REG(b);
      } break;

      case INSTR_ARITH:{
        switch ((ArithModifier)mod)
        {
        case ARITH_ADD:
          REG(a) = REG(b) + REG(c);
          break;
        case ARITH_SUB:
          REG(a) = REG(b) - REG(c);
          break;
        case ARITH_MUL:
          REG(a) = REG(b) * REG(c);
          break;
        case ARITH_DIV:
          if(REG(c) == 0) emu->status = EMU_STATUS_DIV_BY_ZERO;
          else REG(a) = REG(b) / REG(c);
          break;
        
        default:
          emu->status = EMU_STATUS_BAD_MOD;
          break;
        }
      } break;

      case INSTR_LOGIC:{
        switch ((LogicModifier)mod)
        {
        case LOGIC_NOT:
          REG(a) = ~REG(b);
          break;
        case LOGIC_AND:
          REG(a) = REG(b) & REG(c);
          break;
        case LOGIC_OR:
          REG(a) = REG(b) | REG(c);
          break;
        case LOGIC_XOR:
          REG(a) = REG(b) ^ REG(c);
          break;
        default:
          emu->status = EMU_STATUS_BAD_MOD;
          break;
        }
      } break;

      case INSTR_SHIFT:{
        switch ((ShiftModifier)mod)
        {
        case SHIFT_LEFT:
          REG(a) = REG(b) << REG(c);
          break;
        case SHIFT_RIGHT:
          REG(a) = REG(b) >> REG(c);
          break;
        default:
          emu->status = EMU_STATUS_BAD_MOD;
          break;
        }
      }

      case INSTR_STORE:{
        switch ((StoreModifier)mod)
        {
        case STORE_REG_INDIRECT:
          memoryWriteWord(emu, REG(a)+REG(b)+d, REG(c), &isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case STORE_REG_INDIRECT_PREINC:
          memoryWriteWord(emu,(REG(a)+=(REG(b)+d)),REG(c),&isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case STORE_MEM_INDIRECT:
          uint32_t address = memoryReadWord(emu,REG(a)+REG(b)+d,&isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          memoryWriteWord(emu, address, REG(c), &isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          break;
        default:
          emu->status = EMU_STATUS_BAD_MOD;
          break;
        }

      } break;

      case INSTR_LOAD:{
        switch ((LoadModifier)mod)
        {
        case LOAD_CSR:
          if(b < CSR_COUNT) REG(a) = CSR(b);
          else emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case LOAD_REG_DISP:
          REG(a) = REG(b) + d;
          break;
        case LOAD_MEM_INDIRECT:
          REG(a) = memoryReadWord(emu,REG(b) + REG(c) + d,&isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case LOAD_MEM_INDIRECT_POSTINC:
          REG(a) = memoryReadWord(emu,REG(b),&isAligned);
          if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          REG(b) += d;
          break;
        case LOAD_CSR_FROM_REG:
          if(a < CSR_COUNT) CSR(a) = REG(b);
          else emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case LOAD_CSR_OR_DISP:
          if(a < CSR_COUNT && b < CSR_COUNT) CSR(a) = CSR(b)|d;
          else emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case LOAD_CSR_MEM_INDIRECT:
          if(a < CSR_COUNT){        
            CSR(a) = memoryReadWord(emu,REG(b)+REG(c)+d,&isAligned);
            if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
          }
          else emu->status = EMU_STATUS_BUS_ERROR;
          break;
        case LOAD_CSR_MEM_POSTINC:
          if(a < CSR_COUNT){        
            CSR(a) = memoryReadWord(emu,REG(b),&isAligned);
            if(!isAligned)emu->status = EMU_STATUS_BUS_ERROR;
            REG(b)+=d;
          }
          else emu->status = EMU_STATUS_BUS_ERROR;
          break;

        default:
          emu->status = EMU_STATUS_BAD_MOD;
          break;
        }

      } break;

      default: {
        emu->status = EMU_STATUS_BAD_OP;
      } break;
      }
      #if 1
      static const char* instructionOpcodePrint[INSTR_COUNT] = {
        [INSTR_HALT] = "INSTR_HALT",
        [INSTR_INT] = "INSTR_INT",
        [INSTR_CALL] = "INSTR_CALL",
        [INSTR_JMP] = "INSTR_JMP",
        [INSTR_XCHG] = "INSTR_XCHG",
        [INSTR_ARITH] = "INSTR_ARITH",
        [INSTR_LOGIC] = "INSTR_LOGIC",
        [INSTR_SHIFT] = "INSTR_SHIFT",
        [INSTR_STORE] = "INSTR_STORE",
        [INSTR_LOAD] =  "INSTR_LOAD",      
      };
      //printf("Instruction %s executed.\n", instructionOpcodePrint[opcode]);
      /*for(size_t i = 0; i < 16; i++){
        printf("r%-2lu = %08x ", i, REG(i));
        if(i % 8 == 8 - 1) printf("\n");
      }    
      static const char *csr_names[CSR_COUNT] = {
        [CSR_STATUS] = "Status",
        [CSR_HANDLER] = "Handler",
        [CSR_CAUSE] = "Cause",
      };

      for(size_t i = 0; i < CSR_COUNT; i++){
        printf("%-8s = %08x           ", csr_names[i], CSR(i));
      }
      printf("\n");*/

      #endif
    }
    


    static const char *error_print[EMU_STATUS_COUNT] = {
      [EMU_STATUS_SOFTWARE_INTERRUPT] = "Software interrupt",
      [EMU_STATUS_BAD_MOD] = "Bad mod",
      [EMU_STATUS_BAD_OP] = "Bad op",
      [EMU_STATUS_BUS_ERROR] = "Bus error",
      [EMU_STATUS_DIV_BY_ZERO] = "Div by zero",
    };

    switch (emu->status)
    {
    case EMU_STATUS_SOFTWARE_INTERRUPT:
      emulatorRaiseExpection(emu, 4);
      if(emu->status != EMU_STATUS_RUNNING){
        printf("Unrecoverable exception: %s.\n", error_print[emu->status]);
      }
      break;
    case EMU_STATUS_TIMER_INTERRUPT:
      emulatorRaiseExpection(emu, 2);
      if(emu->status != EMU_STATUS_RUNNING){
        printf("Unrecoverable exception: %s.\n", error_print[emu->status]);
      }
      break;
    case EMU_STATUS_BAD_MOD:
    case EMU_STATUS_BAD_OP:
    case EMU_STATUS_BUS_ERROR:
    case EMU_STATUS_DIV_BY_ZERO:
      emulatorRaiseExpection(emu, 1);
      if(emu->status != EMU_STATUS_RUNNING){
        printf("Unrecoverable exception: %s.\n", error_print[emu->status]);
      }
      break;
    case EMU_STATUS_RUNNING:
      // nothing
      break;
    case EMU_STATUS_FINISHED:
      // nothing
      break;
      
    default:
      assert(0);
      break;
    }
  }
  if(emu->status == EMU_STATUS_FINISHED)printf("Program executed sucessfully\n");
  emulatorPrint(emu);
}

void emulatorMemoryTest(Emulator *emu){
  /*  */
  uint32_t addrs[] = {
    0x10200030,
    0x10300020,
    0x20100030,
    0x20300010,
    0x30100020,
    0x30200010,
  };

  for(size_t i = 0; i < arrlen(addrs); i++){
    printf("Setting byte %#10x to %hhu\n", addrs[i], (Byte)i);
    *getMemoryAddr(emu, addrs[i]) = (Byte)i;
  }

  for(size_t i = 0; i < arrlen(addrs); i++){
    printf("Reading byte %#10x as %hhu\n", addrs[i], *getMemoryAddr(emu, addrs[i]));
  }
  uint32_t addrsword = 0x12345678;
  bool isAligned = true;
  memoryWriteWord(emu,addrsword,0x11223344,&isAligned);
  printf("%x \n",memoryReadWord(emu,addrsword,&isAligned));
   

}
