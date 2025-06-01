#include "assembler.h"

struct Assembler *assembler = NULL;

extern FILE *yyin;

enum AssemblerArgsState {
  ASSEMBLER_ARGS_STATE_INPUT,
  ASSEMBLER_ARGS_STATE_OUTPUT,
};

int main(int argc, const char **argv){

  const char *input_path = NULL;
  const char *output_path = NULL;
  enum AssemblerArgsState state = ASSEMBLER_ARGS_STATE_INPUT;

  for(int arg = 1; arg < argc; arg++){
    const char *curr_arg = argv[arg];

    switch(state){
    case ASSEMBLER_ARGS_STATE_INPUT:
      if(strcmp(curr_arg, "-o") == 0) state = ASSEMBLER_ARGS_STATE_OUTPUT;
      else {
        if(input_path == NULL) input_path = curr_arg;
        else {
          printf("Invalid arguments.\n");
          return -1;
        }
      }
      break;
    case ASSEMBLER_ARGS_STATE_OUTPUT:
      if(output_path == NULL){
        output_path = curr_arg;
        state = ASSEMBLER_ARGS_STATE_INPUT;
      }
      else {
        printf("Invalid arguments.\n");
        return -1;
      }
      break;
    }
  }

  if(input_path == NULL){
    printf("Input path not specified.\n");
    return -1;
  }
  yyin = fopen(input_path, "r");

  if(yyin == NULL){
    printf("Cannot open input file.\n");
    return -1;
  }

  if(output_path == NULL){
    printf("Output file not specified.\n");
    return -1;
  }

  FILE *output_file = fopen(output_path, "w");
  if(output_file == NULL){
    printf("Cannot open output file.\n");
    return -1;
  }

  struct Assembler a = assemblerCreate();

  assembler = &a; // assembler

  yyparse();
  assemblerPrint(&a);

  if(a.correct){
    assemblerPrintObjectFile(&a, output_file);
  }
  else {
    printf("Assembler file contains semantic errors.\n");
  }

  assemblerDestroy(&a);

  fclose(yyin);
  fclose(output_file);

  return 0;
}