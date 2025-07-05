#include "emulator.h"

int main(void){
  Emulator emu = emulatorCreate();

  emulatorMemoryTest(&emu);

  emulatorDestroy(&emu);
}