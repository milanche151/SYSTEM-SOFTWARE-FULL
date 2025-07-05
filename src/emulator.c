#include "emulator.h"
#include "util.h"

Emulator emulatorCreate(){
  Emulator em = { 0 };
  em.status = EMU_STATUS_RUNNING;

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
}