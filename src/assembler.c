#include "./inc/assembler.h"


VECTOR_DECLARE(VecSymTbl,symtable);
VECTOR_IMPLEMENT(VecSymTbl,symtable);



symtable* createSymSection(char* symbol, symTableBind bind){
  symtable* sym = (symtable*)malloc(sizeof(symtable));
  if(sym==NULL) {
      printf ("Allocation error.");
      return NULL;
  }
  sym->defined=1;
  sym->name = symbol;  
  sym->section = symbol;
  sym->defined = 1;
  sym->type = SECTION;
  sym->bind = bind;
  return sym;
}

int inserIntoSymbolTable(symtable* sym){
    
  
}
int global(char* symlist){
  
}