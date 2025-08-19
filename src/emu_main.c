#include "emulator.h"
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

// Store original terminal settings so we can restore later
static struct termios orig_termios;

static void disable_raw_mode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void set_nonblocking_mode() {
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

static void enable_raw_mode() {
  struct termios raw;

  // Get current terminal attributes
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disable_raw_mode);  // Ensure raw mode is disabled on exit

  raw = orig_termios;

  printf("ORIG TERMIOS = %x\n", raw.c_lflag);

  // Disable canonical mode, echoing
  // raw.c_lflag &= ~(ECHO | ICANON | ISIG);
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VMIN] = 0;  // No minimum number of characters
  raw.c_cc[VTIME] = 0; // No timeout (0 deciseconds)

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  set_nonblocking_mode();
}

int main(int argc, char** argv){
  enable_raw_mode();
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