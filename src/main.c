#include "assembler.h"

struct Assembler *assembler = NULL;

extern FILE *yyin;

int main(int argc, const char **argv){

  if(argc != 2){
    printf("No input file specified.\n");
    return -1;
  }

  struct Assembler a = assemblerCreate();

  assembler = &a; // assembler
  yyin = fopen(argv[1], "r");
  if(yyin != NULL){
    yyparse();
    assemblerPrint(&a);
  }
  else {
    printf("Cannot open input file.\n");
  }

  assemblerDestroy(&a);
  return 0;
}