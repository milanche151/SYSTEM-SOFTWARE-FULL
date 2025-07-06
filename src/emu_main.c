#include "emulator.h"

int main(int argc, char** argv){
  Emulator emu = emulatorCreate();
  FILE* input = NULL; 
  if(argc == 2){
    input = fopen(argv[1],"r");
  }else{
    printf("Input file not specified.\n");
  }
  
  if(input){
    emulatorLoad(&emu,input);
    emulatorRun(&emu);
  }
 
  //emulatorMemoryTest(&emu);

  emulatorDestroy(&emu);
}