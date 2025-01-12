#include "../inc/assembler.h"

VECTOR_IMPLEMENT(VecSymTbl,SymTableRow);
static int num = 0;
SymTableRow* createSymSection(char* symbol,symTableType type,symTableBind bind){
  SymTableRow* sym = (SymTableRow*)malloc(sizeof(SymTableRow));
  if(sym==NULL) {
      printf ("Allocation error.");
      return NULL;
  }
  sym->defined=1;
  sym->name = symbol;  
  sym->defined = 1;
  sym->type = SECTION;
  sym->bind = bind;
  sym->num = num++;

  return sym;
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
int global(char* symlist){
  char smlist[1024];
  strcpy(smlist,symlist);
  char *p = strtok(smlist, " ");
  SymTableRow *section;
  while(p != NULL) {
    //printf("%s",p);
    section = createSymSection(p,SECTION,GLOBAL);
    inserIntoSymbolTable(section);
    
    p = strtok(NULL, " ");
    
  }
}