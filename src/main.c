#include "../inc/assembler.h"

struct Assembler *assembler = NULL;

int main(void){

  struct Assembler a = assemblerCreate();

  // assembler = &a; // assembler
  // yyparse();

  assemblerDestroy(&a);

}