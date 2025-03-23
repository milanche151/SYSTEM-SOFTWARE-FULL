#include "assembler.h"

struct Assembler *assembler = NULL;

extern FILE *yyin;

int main(void){

  struct Assembler a = assemblerCreate();

  assembler = &a; // assembler
  yyin = fopen("tests/directives.s", "r");
  if(yyin != NULL){
    yyparse();
    assemblerPrint(&a);
  }
  else {
    printf("Cannot open input file.\n");
  }

  assemblerDestroy(&a);

}