#include "assembler.h"
#include <assert.h>
#include "util.h"

VECTOR_IMPLEMENT(VecSymTbl,SymTableRow);
VECTOR_IMPLEMENT(VecString, char*);
VECTOR_IMPLEMENT(VecForwardRef, ForwardRef);
VECTOR_IMPLEMENT(VecRelocation, Relocation);
VECTOR_IMPLEMENT(VecLitPoolEntry, LitPoolEntry);
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

    printf("%-5lu %-10s %-10s %-10d %-10s %-10s\n",
      i,
      row->name,
      assembler->symbolTable.data[row->section].name,
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
    .litPool = VecLitPoolEntryCreate(),
  };
  VecSectionPush(&assembler->sections, new_section);
}

void global(struct Assembler* assembler, VecString symlist){
 for(int i = 0; i < symlist.size; i++){
    declareSymGlobal(assembler, symlist.data[i]);
 }
}

static void
add_data32_reloc(struct Assembler *assembler, Section *current_section, size_t offset, const SymTableRow *symbol){

  bool is_extern = symbol->section == EXTERN_SECTION;
  Relocation relocation = {
    .type = RELOCATION_TYPE_DATA32,
    .symbolIndex = is_extern ? (symbol - &assembler->symbolTable.data[0]) : symbol->section,
    .addend = is_extern ? 0 : symbol->value,
    .offset = offset,
  };
  VecRelocationPush(&current_section->relocations,relocation);
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
          add_data32_reloc(assembler, current_section, current_section->machineCode.size, symbol);
        }
        else {
          ForwardRef forwardRef = {
            .type = FORWARD_REF_DATA32,
            .name = current_expr->name,
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
    
    // align location counter to 4
    size_t padding = current_section->machineCode.size % 4;
    if(padding > 0) padding = 4 - padding;
    
    for(size_t i = 0; i < padding; i++){
      VecBytePush(&current_section->machineCode, 0);
    }

    VecLinePush(&current_section->lines,line);
}

void instructionNoop(struct Assembler *assembler, InstrType instr_type){
  const InstrDesc* desc = instr_descs+instr_type;
  assert(desc->family==INSTR_FAMILY_NOOP);
  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];

    VecBytePush(&current_section->machineCode, (desc->opcode << 4) | (desc->modifier));
    VecBytePush(&current_section->machineCode, 0x00);
    VecBytePush(&current_section->machineCode, 0x00);
    VecBytePush(&current_section->machineCode, 0x00);

    Line line ={
      .type = LINE_TYPE_INSTRUCITON,
      .instruction = {
        .type=instr_type
      }
    };
    VecLinePush(&current_section->lines,line);
  }
  else{
    assembler->correct = false;
  }
  
}

void instructionTworeg(struct Assembler *assembler, InstrType instr_type, int regS, int regD){
  const InstrDesc* desc = instr_descs+instr_type;
  assert(desc->family==INSTR_FAMILY_TWOREG);

  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];

    VecBytePush(&current_section->machineCode, (desc->opcode << 4) | (desc->modifier));
    VecBytePush(&current_section->machineCode, (regD<< 4) | regD);
    VecBytePush(&current_section->machineCode, regS << 4);
    VecBytePush(&current_section->machineCode, 0x00);

    Line line ={
      .type = LINE_TYPE_INSTRUCITON,
      .instruction = {
        .type=instr_type,
        .reg1 = regS,
        .reg2 = regD,
      }
    };
    VecLinePush(&current_section->lines,line);
  }
  else{
    assembler->correct = false;
  }
}

static bool canFitIn12bit(int dist){
  return dist >= -0x0800 && dist < 0x0800;
}

// this register's value is always 0 (reg0's value is always 0)
#define REGISTER_ZERO 0

void instructionLoadStore(struct Assembler *assembler,InstrType instrType, Operand operand, int regD){
  const InstrDesc *desc = instr_descs+INSTR_LD;
  assert(desc->family == INSTR_FAMILY_LD);

  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];
    
    // generate LitPoolEntry
    switch(operand.type){

    case OPERAND_TYPE_IMMED_LIT:
      VecForwardRefPush(&current_section->forwardRefs,(ForwardRef){
        .offset = current_section->machineCode.size,
        .lit_idx = current_section->litPool.size,
        .type = FORWARD_REF_LITPOOL_NUM,
      });
      VecLitPoolEntryPush(&current_section->litPool, (LitPoolEntry){.value = operand.literal} );
      break;

    case OPERAND_TYPE_IMMED_SYM:
      VecForwardRefPush(&current_section->forwardRefs,(ForwardRef){
        .offset = current_section->machineCode.size,
        .lit_idx = current_section->litPool.size,
        .type = FORWARD_REF_LITPOOL_SYM,
        .name = operand.symbol
      });
      VecLitPoolEntryPush(&current_section->litPool, (LitPoolEntry){.value = 0x00 } );
      break;

    case OPERAND_TYPE_MEMDIR_LIT:
      VecForwardRefPush(&current_section->forwardRefs,(ForwardRef){
        .offset = current_section->machineCode.size,
        .lit_idx = current_section->litPool.size,
        .type = FORWARD_REF_LITPOOL_NUM,
      });
      VecLitPoolEntryPush(&current_section->litPool, (LitPoolEntry){.value = operand.literal} );
      break;

    case OPERAND_TYPE_MEMDIR_SYM:
      VecForwardRefPush(&current_section->forwardRefs,(ForwardRef){
        .offset = current_section->machineCode.size,
        .lit_idx = current_section->litPool.size,
        .type = FORWARD_REF_LITPOOL_SYM,
        .name = operand.symbol
      });
      VecLitPoolEntryPush(&current_section->litPool, (LitPoolEntry){.value = 0x00 } );
      break;

    case OPERAND_TYPE_REGDIR:
    case OPERAND_TYPE_REGIND:
    case OPERAND_TYPE_REGIND_LIT:
    case OPERAND_TYPE_REGIND_SYM:
      // no need for literal pool entry
      break;
    default: assert(0);
    }
    if(instrType == INSTR_LD){
      switch(operand.type){
      case OPERAND_TYPE_IMMED_LIT:
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x01);
        VecBytePush(&current_section->machineCode, regD<<4 | REGISTER_ZERO);
        VecBytePush(&current_section->machineCode, 0x00 << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_IMMED_SYM:
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x01);
        VecBytePush(&current_section->machineCode, regD<<4 | REGISTER_ZERO);
        VecBytePush(&current_section->machineCode, 0x00 << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_MEMDIR_LIT:  
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x02);
        VecBytePush(&current_section->machineCode, regD<<4 | REGISTER_ZERO);
        VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_MEMDIR_SYM: 
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x02);
        VecBytePush(&current_section->machineCode, regD<<4 | REGISTER_ZERO);
        VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_REGDIR:
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x01);
        VecBytePush(&current_section->machineCode, regD<<4 | operand.reg);
        VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_REGIND:
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x02);
        VecBytePush(&current_section->machineCode, regD<<4 | operand.reg);
        VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_REGIND_LIT:
        if(canFitIn12bit(operand.literal)){ 
          VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x02);
          VecBytePush(&current_section->machineCode, regD<<4 | operand.reg);
          VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | (operand.literal>>8 & 0x0f));
          VecBytePush(&current_section->machineCode, operand.literal & 0xff);
        }
        else {
          assembler->correct = false;
        }
        break;
      case OPERAND_TYPE_REGIND_SYM:
        assembler->correct = false;
        break;
      default:assert(0);
      }
    }
    else if(instrType == INSTR_STR){
      switch(operand.type){
      case OPERAND_TYPE_IMMED_LIT:
      case OPERAND_TYPE_IMMED_SYM:
        assembler->correct = false;
        break;
      case OPERAND_TYPE_MEMDIR_LIT:  
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x00);
        VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | REGISTER_ZERO);
        VecBytePush(&current_section->machineCode, regD << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_MEMDIR_SYM: 
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x00);
        VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | REGISTER_ZERO);
        VecBytePush(&current_section->machineCode, regD << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_REGDIR:
        assembler->correct = false;
        break;
      case OPERAND_TYPE_REGIND:
        VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x00);
        VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | operand.reg);
        VecBytePush(&current_section->machineCode, regD << 4 | 0x00);
        VecBytePush(&current_section->machineCode, 0x00);
        break;
      case OPERAND_TYPE_REGIND_LIT:
        if(canFitIn12bit(operand.literal)){ 
          VecBytePush(&current_section->machineCode, desc->opcode<<4 | 0x00);
          VecBytePush(&current_section->machineCode, REGISTER_ZERO << 4 | operand.reg);
          VecBytePush(&current_section->machineCode, regD << 4 | (operand.literal>>8 & 0x0f));
          VecBytePush(&current_section->machineCode, operand.literal & 0xff);
        }
        else {
          assembler->correct = false;
        }
        break;
      case OPERAND_TYPE_REGIND_SYM:
        assembler->correct = false;
        break;
      }
    }
    else{
      assert(0);
    }
    


    Line line ={
      .type = LINE_TYPE_INSTRUCITON,
      .instruction = {
        .type = instrType,
        .operand = operand,
        .reg2 = regD,
      }
    };
    VecLinePush(&current_section->lines,line);
  }
  else {
    assembler->correct = true;
  }
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
      
    default: assert(0);
    }
    break;
  case LINE_TYPE_INSTRUCITON: {
    const InstrDesc *desc = &instr_descs[line->instruction.type];
    printf("%s ", desc->name);
    switch(desc->family){
    case INSTR_FAMILY_NOOP:
      break;
    case INSTR_FAMILY_TWOREG:
      printf("%%r%d, %%r%d", line->instruction.reg1, line->instruction.reg2);
      break;
    case INSTR_FAMILY_LD:
      printf("LOAD INSTR");
      break;
    case INSTR_FAMILY_STR:
      printf("STORE INSTR");
      break;
    default: assert(0);
    }
  } break;
  default: assert(0);
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
    printf("Section size = %lu (+%lu bytes for literals)\n", section->machineCode.size, section->litPool.size * 4);

    size_t n_print = 0;

    for(size_t j = 0; j < section->machineCode.size; j++){
      printf("%02x", section->machineCode.data[j]);
      n_print += 1;
      if(n_print % 8 == 0) printf("\n");
      else printf(" ");
    }
    for(size_t j = 0; j < section->litPool.size; j++){
      printf("%02x %02x %02x %02x",
        (section->litPool.data[j].value >> 0) & 0xff,
        (section->litPool.data[j].value >> 8) & 0xff,
        (section->litPool.data[j].value >> 16) & 0xff,
        (section->litPool.data[j].value >> 24) & 0xff
      );
      n_print += 4;
      if(n_print % 8 == 0) printf("\n");
      else printf(" ");
    }
    printf("\n");
    printf("RELOCATIONS:\n");
    static const char* relocationTypePrint[RELOACTION_TYPE_COUNT] = {
          [RELOCATION_TYPE_DATA32]= "DATA32"
    };
    printf("%-10s %-10s %-10s %-5s\n",
      "Offset",
      "Type",
      "Symbol",
      "Addend"
    );
    for(size_t j = 0; j < section->relocations.size; j++){
      const Relocation *current_rel = &section->relocations.data[j];
      printf("%-10lu %-10s %-10s %-5d\n",
              current_rel->offset,
              relocationTypePrint[current_rel->type],
              assembler->symbolTable.data[current_rel->symbolIndex].name,
              current_rel->addend
            );
      
    }
    printf("\n");
  }
}

void AssemblerEndOfFile(struct Assembler *assembler){
  for (size_t i = 0; i < assembler->sections.size; i++)
  {
    Section* currentSection = &assembler->sections.data[i];
    for (size_t j = 0; j < currentSection->forwardRefs.size; j++)
    {
      const ForwardRef* currentFR = &currentSection->forwardRefs.data[j];


      switch (currentFR->type)
      {
      case FORWARD_REF_DATA32:{

        SymTableRow *symbol = SymTableFind(assembler, currentFR->name);
        
        if(symbol){
          add_data32_reloc(assembler, currentSection, currentFR->offset, symbol);
        }
        else {
          assembler->correct = false;
        }
      }
      break;
      case FORWARD_REF_LITPOOL_NUM:{
        int dist = currentSection->machineCode.size - (currentFR->offset+4) + currentFR->lit_idx*4;
        if(canFitIn12bit(dist)){
          unsigned char *second = &currentSection->machineCode.data[currentFR->offset + 2];
          unsigned char *third  = &currentSection->machineCode.data[currentFR->offset + 3];

          *second &= 0xF0;
          *third &= 0x00;
          *second |= (dist>>8)&(0x0F);
          *third |= (dist)&(0xFF);
        }
        else {
          assembler->correct = false;
        }
      }
      break;
      case FORWARD_REF_LITPOOL_SYM:{
        int dist = currentSection->machineCode.size - (currentFR->offset+4) + currentFR->lit_idx*4;
        if(canFitIn12bit(dist)){
          unsigned char *second = &currentSection->machineCode.data[currentFR->offset + 2];
          unsigned char *third  = &currentSection->machineCode.data[currentFR->offset + 3];

          *second &= 0xF0;
          *third &= 0x00;
          *second |= (dist>>8)&(0x0F);
          *third |= (dist)&(0xFF);
        }
        else {
          assembler->correct = false;
        }

        SymTableRow *symbol = SymTableFind(assembler, currentFR->name);
        if(symbol){
          add_data32_reloc(assembler, currentSection, currentSection->machineCode.size + currentFR->lit_idx * 4, symbol);
        }
        else {
          assembler->correct = false;
        }
      }
      break;
      default:
        assert(0);
        break;
      }

    }
    
  }
  
}