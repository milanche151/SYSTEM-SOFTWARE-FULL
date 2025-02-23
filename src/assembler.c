#include "../inc/assembler.h"

VECTOR_IMPLEMENT(VecSymTbl,SymTableRow);
VECTOR_IMPLEMENT(VecString, char*);
static int num = 0;
SymTableRow* createSymSection(char* symbol,symTableBind bind){
  SymTableRow* sym = (SymTableRow*)malloc(sizeof(SymTableRow));
  if(sym==NULL) {
      printf ("Allocation error.");
      return NULL;
  }
  sym->name = symbol;  
  sym->defined = 1;
  sym->type = SECTION;
  sym->bind = bind;
  sym->num = num++;

  return sym;
}
SymTableRow* createSymbol(char* symbol,symTableBind bind,int defined){
  SymTableRow* sym = (SymTableRow*)malloc(sizeof(SymTableRow));
  if(sym==NULL) {
      printf ("Allocation error.");
      return NULL;
  }
  sym->defined=0;
  sym->name = symbol;  
  sym->type = NOTYPE;
  sym->bind = bind;
  sym->num = num++;

}
int inserIntoSymbolTable(SymTableRow* sym){
     VecSymTblPush(&symbolTable, *sym);
}
void initSymbolTable(){
  symbolTable=VecSymTblCreate();
}
void printSymTable(){
  for(int i = 0; i < symbolTable.size; i++){
    printf("%d %s\n",symbolTable.data[i].num,symbolTable.data[i].name);
  }
}
int global(VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    createSymbol(symlist.data[i],GLOBAL,1);
 }
}
int word(VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    createSymbol(symlist.data[i],LOCAL,1);
 }
}
int externSym(VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    createSymbol(symlist.data[i],GLOBAL,0);
 }
}
