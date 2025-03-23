#include "assembler.h"

#include "util.h"

VECTOR_IMPLEMENT(VecSymTbl,SymTableRow);
VECTOR_IMPLEMENT(VecString, char*);
VECTOR_IMPLEMENT(VecSection, Section);
VECTOR_IMPLEMENT(VecExpr,Expression);
VECTOR_IMPLEMENT(VecLine,Line);

static int num = 0;
SymTableRow createSymSection(struct Assembler* assembler, char* symbol,symTableBind bind){
  SymTableRow sym = {
  .name = symbol,  
  .defined = true,
  .type = SYM_TBL_TYPE_SECTION,
  .bind = bind,
  .num = num++,
  };

  return sym;
}

SymTableRow createSymbol(struct Assembler* assembler, char* symbol,symTableBind bind,bool defined){
  SymTableRow sym = {
    .defined=defined,
    .name = symbol,
    .type = SYM_TBL_TYPE_NOTYPE,
    .bind = bind,
    .num = num++,
  }; 

  return sym;
}

void insertIntoSymbolTable(struct Assembler* assembler, SymTableRow sym){
  VecSymTblPush(&assembler->symbolTable, sym);
}

void initSymbolTable(struct Assembler* assembler){
  assembler->symbolTable=VecSymTblCreate();
}

void printSymTable(const struct Assembler* assembler){

  const char *symbol_type_print[SYM_TBL_TYPE_COUNT] = {
    [SYM_TBL_TYPE_NOTYPE]   = "NoType",
    [SYM_TBL_TYPE_FILE_T]   = "File",
    [SYM_TBL_TYPE_SECTION]  = "Section",
    [SYM_TBL_TYPE_OBJECT]   = "Object",
    [SYM_TBL_TYPE_FUNCTION] = "Function",
  };

  printf("Symtab:\n");
  printf("%5s %10s %10s %10s %10s %10s\n",
    "Num",
    "Name",
    "Value",
    "Type",
    "Offset",
    "Status"
  );
  for(int i = 0; i < assembler->symbolTable.size; i++){
    const SymTableRow *row = &assembler->symbolTable.data[i];

    printf("%5d %10s %10d %10s %10d %10s\n",
      row->num,
      row->name,
      row->value,
      symbol_type_print[row->type],
      row->offset,
      row->defined ? "Defined" : "Not defined"
    );
  }
}

void section(struct Assembler* assembler, char* symbol){
  SymTableRow sym = createSymSection(assembler,symbol,BIND_TYPE_LOCAL);
  insertIntoSymbolTable(assembler,sym);
}

void global(struct Assembler* assembler, VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    insertIntoSymbolTable(assembler, createSymbol(assembler,symlist.data[i],BIND_TYPE_GLOBAL,0));
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
    insertIntoSymbolTable(assembler, createSymbol(assembler,symlist.data[i],BIND_TYPE_GLOBAL,1));
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



static void exprPrint(const Expression* expr){
  switch(expr->type){
  case EXPR_TYPE_NUMBER:
    printf("%d", expr->val);
    break;

  case EXPR_TYPE_SYMBOL:
    printf("%s", expr->name);
    break;

  }
}

static void linePrint(const Line* line){
  static const char *directiveNames[] = {
    [DIRECTIVE_TYPE_WORD]  = ".word",
    [DIRECTIVE_TYPE_SKIP]  = ".skip",
    [DIRECTIVE_TYPE_ASCII] = ".ascii",
  };
  switch (line->type)
  {
  case LINE_TYPE_DIRECTIVE:
    printf("%s ", directiveNames[line->directive.type]);
    switch(line->directive.type){
    case DIRECTIVE_TYPE_WORD:
    case DIRECTIVE_TYPE_SKIP:{
      for (size_t i = 0; i < line->directive.expressions.size; i++)
      {
        exprPrint(&line->directive.expressions.data[i]);
      }
    } break;

    case DIRECTIVE_TYPE_ASCII:
      break;
      
    }
    break;
  case LINE_TYPE_INSTRUCITON:
    /* TODO */
    break;
  default:
    break;
  }
  printf("\n");

}

void assemblerPrint(const struct Assembler* assembler){
  printf("ASSEMBLER PRINT:\n");

  printSymTable(assembler);
  for(size_t i = 0; i<assembler->sections.size; i++){
    const Section* section = &assembler->sections.data[i];
    for(size_t j = 0; j < section->lines.size; j++){
      const Line *currentLine = &section->lines.data[j];
      linePrint(currentLine);
    }
  }
}