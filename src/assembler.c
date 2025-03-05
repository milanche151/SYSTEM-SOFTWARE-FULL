#include "../inc/assembler.h"

VECTOR_IMPLEMENT(VecSymTbl,SymTableRow);
VECTOR_IMPLEMENT(VecString, char*);
VECTOR_IMPLEMENT(VecSection, Section);
VECTOR_IMPLEMENT(VecExpr,Expression);
VECTOR_IMPLEMENT(VecLine,Line);

static int num = 0;
SymTableRow* createSymSection(struct Assembler* assembler, char* symbol,symTableBind bind){
  SymTableRow* sym = (SymTableRow*)myMalloc(sizeof(SymTableRow));
  sym->name = symbol;  
  sym->defined = true;
  sym->type = SECTION;
  sym->bind = bind;
  sym->num = num++;
  return sym;
}

SymTableRow* createSymbol(struct Assembler* assembler, char* symbol,symTableBind bind,bool defined){
  SymTableRow* sym = (SymTableRow*)myMalloc(sizeof(SymTableRow));
  sym->defined=defined;
  sym->name = symbol;  
  sym->type = NOTYPE;
  sym->bind = bind;
  sym->num = num++;

}

void inserIntoSymbolTable(struct Assembler* assembler, SymTableRow* sym){
     VecSymTblPush(&assembler->symbolTable, *sym);
}

void initSymbolTable(struct Assembler* assembler){
  assembler->symbolTable=VecSymTblCreate();
}

void printSymTable(struct Assembler* assembler){
  for(int i = 0; i < &assembler->symbolTable.size; i++){
    printf("%d %s\n",&assembler->symbolTable.data[i].num,&assembler->symbolTable.data[i].name);
  }
}

void global(struct Assembler* assembler, VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    createSymbol(assembler,symlist.data[i],GLOBAL,0);
 }
}

void word(struct Assembler* assembler, VecExpr expresions){
  if(assembler,VecSectionIsEmpty(&assembler->sections)){
    assembler->correct=false;
  }
  else{
    Section *current_section = &assembler->sections.data[assembler->sections.size - 1];
    Line line  = {
      .type = LINE_TYPE_DIRECTIVE,
      .directive = {
        .type = DIRECTIVE_TYPE_WORD,
        .expressions = expresions,
      },
    };
    current_section->locationCounter+=4;
    VecLinePush(&current_section->lines,line);
  }
}

void externSym(struct Assembler* assembler,VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    createSymbol(assembler,symlist.data[i],GLOBAL,1);
 }
}

void ascii(struct Assembler* assembler, char* string){
  Section *current_section = &assembler->sections.data[assembler->sections.size - 1];
    Line line  = {
      .type = LINE_TYPE_DIRECTIVE,
      .directive = {
        .type = DIRECTIVE_TYPE_ASCII,
        .string = string,
      },
    };
    current_section->locationCounter+=4;
    VecLinePush(&current_section->lines,line);
}

struct Assembler
assemblerCreate(void){
  return (struct Assembler){
    .sections = VecSectionCreate(),
    .symbolTable = VecSymTblCreate(),
    .correct = true,
  };
}

void assemblerDestroy(struct Assembler *assembler){
  for(int i=0;i<assembler->sections.size;i++){
      Section* curr_section = &assembler->sections.data[i];
      Line* lines = curr_section->lines.data;
      for (size_t i = 0; i < curr_section->lines.size; i++)
      {
        myFree(lines[i].directive.expressions.data);
      }
      myFree(lines);
  }
  VecSectionDestroy(&assembler->sections);
  VecSymTblDestroy(&assembler->symbolTable);
}
