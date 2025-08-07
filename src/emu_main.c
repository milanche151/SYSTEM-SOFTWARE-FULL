#include "emulator.h"
#include <time.h>

int main(int argc, char** argv){
  clock_t start_time, end_time;
  Emulator emu = emulatorCreate();
  FILE* input = NULL; 
  if(argc == 2){
    input = fopen(argv[1],"r");
  }else{
    printf("Input file not specified.\n");
  }
  start_time = clock();
  if(input){
    emulatorLoad(&emu,input);
    emulatorRun(&emu);
  }
  end_time = clock();
  double elapsed_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  printf("Elapsed time is %lf seconds\n", elapsed_seconds);
  //emulatorMemoryTest(&emu);
  emulatorDestroy(&emu);
}