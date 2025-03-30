#include "assembler.h"

#include "util.h"

VECTOR_IMPLEMENT(VecSymTbl,SymTableRow);
VECTOR_IMPLEMENT(VecString, char*);
VECTOR_IMPLEMENT(VecSection, Section);
VECTOR_IMPLEMENT(VecExpr,Expression);
VECTOR_IMPLEMENT(VecLine,Line);
VECTOR_IMPLEMENT(VecByte, unsigned char);

#define EXTERN_SECTION 0

void insertSymSection(struct Assembler* assembler, char* name){
  SymTableRow* found = NULL;
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    SymTableRow* current = &assembler->symbolTable.data[i];

    if(strcmp(current->name, name) == 0) {
      found = current;
      break;
    }
  }

  if(found != NULL){
    printf("ERROR: Section symbol already exists.\n");
    assembler->correct = false;
  }
  else {
    VecSymTblPush(
      &assembler->symbolTable,
      (SymTableRow){
        .name = name,
        .section = assembler->symbolTable.size,
        .value = 0,
        .type = SYM_TBL_TYPE_SECTION,
        .defined = true,
        .bind = BIND_TYPE_LOCAL,
      }
    );
  }
}

void insertSymLabel(struct Assembler* assembler, char* name){
  SymTableRow* found = NULL;
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    SymTableRow* current = &assembler->symbolTable.data[i];

    if(strcmp(current->name, name) == 0) {
      found = current;
      break;
    }
  }

  if(assembler->sections.size > 0){
    Section *current_section = &assembler->sections.data[assembler->sections.size - 1];

    if(found != NULL){
      // okay
      if(!found->defined) {
        found->section = current_section->symtabIndex;
        found->value = current_section->machineCode.size;
        found->defined = true;
      }
      // error
      else {
        printf("ERROR: Label symbol already defined.\n");
        assembler->correct = false;
      }
    }
    else {
      VecSymTblPush(
        &assembler->symbolTable,
        (SymTableRow){
          .name = name,
          .section = current_section->symtabIndex,
          .value = current_section->machineCode.size,
          .type = SYM_TBL_TYPE_NOTYPE,
          .defined = true,
          .bind = BIND_TYPE_LOCAL
        }
      );
    }
  }
  // no section is open
  else {
    printf("ERROR: No section is open.\n");
    assembler->correct = false;
  }
}

void insertSymExtern(struct Assembler* assembler, char *name){
  SymTableRow* found = NULL;
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    SymTableRow* current = &assembler->symbolTable.data[i];

    if(strcmp(current->name, name) == 0) {
      found = current;
      break;
    }
  }
  
  if(found != NULL){
    printf("ERROR: Extern symbol already defined.\n");
    assembler->correct = false;
  }
  else {
    VecSymTblPush(
      &assembler->symbolTable,
      (SymTableRow){
        .name = name,
        .section = EXTERN_SECTION,
        .value = 0,
        .type = SYM_TBL_TYPE_NOTYPE,
        .defined = true,
        .bind = BIND_TYPE_LOCAL,
      }
    );
  }
}

void declareSymGlobal(struct Assembler* assembler, char *name){
  SymTableRow* found = NULL;
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    SymTableRow* current = &assembler->symbolTable.data[i];

    if(strcmp(current->name, name) == 0) {
      found = current;
      break;
    }
  }

  if(found != NULL){
    // extern symbol CAN'T be declared global
    if(found->section == EXTERN_SECTION){
      printf("ERROR: Extern symbol can't be declared global.\n");
      assembler->correct = false;
    }
    // section CAN'T be global
    else if(found->type == SYM_TBL_TYPE_SECTION){
      printf("ERROR: Section symbol can't be declared global.\n");
      assembler->correct = false;
    }
    // okay
    else {
      found->bind = BIND_TYPE_GLOBAL;
    }
  }
  else {

    VecSymTblPush(
      &assembler->symbolTable,
      (SymTableRow){
        .name = name,
        .section = 0,
        .value = 0,
        .type = SYM_TBL_TYPE_NOTYPE,
        .defined = false,
        .bind = BIND_TYPE_GLOBAL
      }
    );
  }
}

static SymTableRow *SymTableFind(const struct Assembler *assembler, const char *name){
  SymTableRow* found = NULL;
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    SymTableRow* current = &assembler->symbolTable.data[i];

    if(strcmp(current->name, name) == 0) {
      found = current;
      break;
    }
  }
  return found;
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
  printf("%-5s %-10s %-10s %-10s %-10s %-10s\n",
    "Num",
    "Name",
    "Section",
    "Value",
    "Type",
    "Status"
  );
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    const SymTableRow *row = &assembler->symbolTable.data[i];

    printf("%-5lu %-10s %-10lu %-10d %-10s %-10s\n",
      i,
      row->name,
      row->section,
      row->value,
      symbol_type_print[row->type],
      row->defined ? "Defined" : "Not def"
    );
  }
}

void section(struct Assembler* assembler, char* symbol){
  insertSymSection(assembler,symbol);

  Section new_section = {
    .symtabIndex = assembler->symbolTable.size - 1,
    .machineCode = VecByteCreate(),
    .lines = VecLineCreate(),
    .forwardRefs = VecForwardRefCreate(),
    .relocations = VecRelocationCreate(),
  };
  VecSectionPush(&assembler->sections, new_section);
}

void global(struct Assembler* assembler, VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    declareSymGlobal(assembler, symlist.data[i]);
 }
}

void word(struct Assembler* assembler, VecExpr expressions){
  if(VecSectionIsEmpty(&assembler->sections)){
    assembler->correct=false;
  }
  else{
    Section *current_section = &assembler->sections.data[assembler->sections.size - 1];
    Line line  = {
      .type = LINE_TYPE_DIRECTIVE,
      .directive = {
        .type = DIRECTIVE_TYPE_WORD,
        .expressions = expressions,
      },
    };

    for(size_t i = 0; i < expressions.size; i++){
      Expression* current_expr = &expressions.data[i];
      if(current_expr->type == EXPR_TYPE_SYMBOL){
        SymTableRow* symbol = SymTableFind(assembler,current_expr->name);
        if(symbol){
          Relocation relocation = {
            .type = RELOCATION_TYPE_DATA32,
            .symbolIndex = symbol->section,
            .addend = symbol->value,
            .offset = current_section->machineCode.size,
          };
          VecRelocationPush(&current_section->relocations,relocation);
        }
        else {
          ForwardRef forwardRef = {
            .type = FORWARD_REF_DATA32,
            .name = symbol->name,
            .addend = 0,
            .offset = current_section->machineCode.size
          };
          VecForwardRefPush(&current_section->forwardRefs,forwardRef);
        }

        for(int i = 0; i < 4; i++){
          VecBytePush(&current_section->machineCode, 0);
        }
        
      }
      else {
        for(int i = 0; i < 4; i++){
          VecBytePush(&current_section->machineCode, current_expr->val >> (8 * i));
        }
      }
    }
    VecLinePush(&current_section->lines,line);
  }
}

void externSym(struct Assembler* assembler,VecString symlist){
  for(int i = 0; i < symlist.size; i++){
    insertSymExtern(assembler, symlist.data[i]);
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
    for(char *p = string; *p != 0; p++){
      if(*p != '"') VecBytePush(&current_section->machineCode, *p);
    }
    VecBytePush(&current_section->machineCode, 0);
    
    VecLinePush(&current_section->lines,line);
}

struct Assembler
assemblerCreate(void){
  struct Assembler assembler = (struct Assembler){
    .sections = VecSectionCreate(),
    .symbolTable = VecSymTblCreate(),
    .correct = true,
  };

  insertSymSection(&assembler,"*UNDEF*");
  
  return assembler;
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
  printf("MACHINE CODE:\n");
  for(size_t i = 0; i < assembler->sections.size; i++){
    const Section* section = &assembler->sections.data[i];
    printf("Section index = %lu\n", section->symtabIndex);
    printf("Section size = %lu\n", section->machineCode.size);
    for(size_t j = 0; j < section->machineCode.size; j++){
      printf("%02x", section->machineCode.data[j]);
      if(j % 8 == 7) printf("\n");
      else printf(" ");
    }
  }
}