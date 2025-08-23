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
VECTOR_IMPLEMENT(VecEquExpr, EquExpr);

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

void insertSym(struct Assembler *assembler, char *name, size_t section_index, CORE_ADDR value){
  SymTableRow* found = NULL;
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    SymTableRow* current = &assembler->symbolTable.data[i];

    if(strcmp(current->name, name) == 0) {
      found = current;
      break;
    }
  }

  if(found != NULL){
    // okay
    if(!found->defined) {
      found->section = section_index;
      found->value = value;
      found->defined = true;
    }
    // error
    else {
      printf("ERROR: Symbol %s already defined.\n", name);
      assembler->correct = false;
    }
  }
  else {
    VecSymTblPush(
      &assembler->symbolTable,
      (SymTableRow){
        .name = name,
        .section = section_index,
        .value = value,
        .type = SYM_TBL_TYPE_NOTYPE,
        .defined = true,
        .bind = BIND_TYPE_LOCAL
      }
    );
  }
}

void insertSymLabel(struct Assembler *assembler, char *name){
  if(assembler->sections.size > 0){
    Section *current_section = &assembler->sections.data[assembler->sections.size - 1];

    insertSym(assembler, name, current_section->symtabIndex, current_section->machineCode.size);
  }
  // no section is open
  else {
    printf("ERROR: No section is open.\n");
    assembler->correct = false;
  }
}

#if 0
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
#endif

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

  const char *symbol_bind_print[BIND_TYPE_COUNT] = {
    [BIND_TYPE_LOCAL] = "LOCAL",
    [BIND_TYPE_GLOBAL] = "GLOBAL",
  };

  printf("Symtab:\n");
  printf("%-5s %-10s %-10s %-10s %-10s %-10s %-10s\n",
    "Num",
    "Name",
    "Section",
    "Binding",
    "Value",
    "Type",
    "Status"
  );

  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    const SymTableRow *row = &assembler->symbolTable.data[i];

    printf("%-5lu %-10s %-10s %-10s %-10d %-10s %-10s\n",
      i,
      row->name,
      assembler->symbolTable.data[row->section].name,
      symbol_bind_print[row->bind],
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

void equ(struct Assembler* assembler, char* name, Expression expr){
  VecEquExprPush(&assembler->equExprs,(EquExpr){ .name = name, .value = expr, .resolved = false });
}

void ascii(struct Assembler* assembler, char* string){
  if(assembler->sections.size > 0){
    Section *current_section = &assembler->sections.data[assembler->sections.size - 1];
    Line line  = {
      .type = LINE_TYPE_DIRECTIVE,
      .directive = {
        .type = DIRECTIVE_TYPE_ASCII,
        .string = string,
      },
    };
    
    bool has_escape = false;
    for(char *p = string; *p != 0; p++){
      if(*p == '\\') {
        has_escape = true;
      }
      else if(*p != '"') {
        if(has_escape){
          assert(*p == 'n');
          VecBytePush(&current_section->machineCode, '\n');
        }
        else {
          VecBytePush(&current_section->machineCode, *p);
        }
        has_escape = false;
      }
      else {
        // nothing
      }
    }
    VecBytePush(&current_section->machineCode, 0);
    
    // align location counter to 4
    size_t padding = current_section->machineCode.size % 4;
    if(padding > 0) padding = 4 - padding;
    
    for(size_t i = 0; i < padding; i++){
      VecBytePush(&current_section->machineCode, 0);
    }

    VecLinePush(&current_section->lines,line);
  }else{
    printf("Directive placed outside of section.\n");
    assembler->correct = false;
  }
}

// this register's value is always 0 (reg0's value is always 0)
#define REGISTER_ZERO 0x0
#define REGISTER_SP 0xE
#define REGISTER_PC 0xF
#define REGISTER_CSR_STATUS 0x0

void insertGenericInstruction(struct Assembler *assembler, int opcode, int modifier, int gprA, int gprB, int gprC, int disp){
  assert(opcode >= 0x00 && opcode < 0x10);
  assert(modifier >= 0x00 && modifier < 0x10);
  assert(gprA >= 0x00 && gprA < 0x10);
  assert(gprB >= 0x00 && gprB < 0x10);
  assert(gprC >= 0x00 && gprC < 0x10);
  assert(disp >= -0x0800 && disp < 0x0800);

  assert(assembler->sections.size > 0);

  Section* current_section = &assembler->sections.data[assembler->sections.size - 1];

  VecBytePush(&current_section->machineCode, (opcode << 4) & 0xf0 | (modifier << 0) & 0x0f);
  VecBytePush(&current_section->machineCode, (gprA   << 4) & 0xf0 | (gprB     << 0) & 0x0f);
  VecBytePush(&current_section->machineCode, (gprC   << 4) & 0xf0 | (disp     >> 8) & 0x0f);
  VecBytePush(&current_section->machineCode, (disp   >> 0) & 0xff);
}

void instructionNoop(struct Assembler *assembler, InstrType instr_type){
  const InstrDesc* desc = instr_descs+instr_type;
  assert(desc->family==INSTR_FAMILY_NOOP);
  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];

    insertGenericInstruction(assembler, desc->opcode, desc->modifier, 0, 0, 0, 0);

    Line line ={
      .type = LINE_TYPE_INSTRUCITON,
      .instruction = {
        .type=instr_type
      }
    };
    VecLinePush(&current_section->lines,line);
  }
  else{
    printf("No sections defined.\n");
    assembler->correct = false;
  }
  
}

#define STACK_DISP 4

void instructionRet(struct Assembler *assembler){
  if(assembler->sections.size > 0){
    insertGenericInstruction(assembler, 0x09, 0x03, REGISTER_PC, REGISTER_SP, 0, +STACK_DISP);
  }
  else {
    printf("No sections defined.\n");
    assembler->correct = false;
  }
}

void instructionIret(struct Assembler *assembler){
  if(assembler->sections.size > 0){

    insertGenericInstruction(assembler, 0x09, 0x06, REGISTER_CSR_STATUS, REGISTER_SP, REGISTER_ZERO, +1 * STACK_DISP);
    insertGenericInstruction(assembler, 0x09, 0x03, REGISTER_PC, REGISTER_SP, REGISTER_ZERO, +2 * STACK_DISP);
  }

  else {
    printf("No sections defined.");
    assembler->correct = false;
  }
}

void instructionOnereg(struct Assembler *assembler, InstrType instr_type, int reg){
  const InstrDesc* desc = instr_descs+instr_type;
  assert(desc->family==INSTR_FAMILY_ONEREG);

  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];

    if(instr_type == INSTR_PUSH){
      insertGenericInstruction(assembler, desc->opcode, desc->modifier, REGISTER_SP, 0, reg, -STACK_DISP);
    }
    else if(instr_type == INSTR_POP){
      insertGenericInstruction(assembler, desc->opcode, desc->modifier, reg, REGISTER_SP, 0, +STACK_DISP);
    }
    else assert(0);

    Line line = {
      .type = LINE_TYPE_INSTRUCITON,
      .instruction = {
        .type=instr_type,
        .reg1 = reg,
      }
    };
    VecLinePush(&current_section->lines,line);
  }
  else{
    printf("No sections defined.\n");
    assembler->correct = false;
  }
}

void instructionTworeg(struct Assembler *assembler, InstrType instr_type, int regS, int regD){
  const InstrDesc* desc = instr_descs+instr_type;
  assert(desc->family==INSTR_FAMILY_TWOREG);

  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];
    
    insertGenericInstruction(assembler, desc->opcode, desc->modifier, regD, regD, regS, 0);

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
    printf("No sections defined.\n");
    assembler->correct = false;
  }
}

static bool canFitIn12bit(int dist){
  return dist >= -0x0800 && dist < 0x0800;
}

void instructionLoadStore(struct Assembler *assembler,InstrType instrType, Operand operand, int regD){
  const InstrDesc *desc = instr_descs+instrType;
  assert(desc->family == INSTR_FAMILY_LD || desc->family == INSTR_FAMILY_STR);

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
        insertGenericInstruction(assembler, desc->opcode, 0x02, regD, REGISTER_PC, REGISTER_ZERO, 0);
        break;
      case OPERAND_TYPE_IMMED_SYM:
        insertGenericInstruction(assembler, desc->opcode, 0x02, regD, REGISTER_PC, REGISTER_ZERO, 0);
        break;
      case OPERAND_TYPE_MEMDIR_LIT:
        insertGenericInstruction(assembler, desc->opcode, 0x02, regD, REGISTER_PC, REGISTER_ZERO, 0);
        insertGenericInstruction(assembler, desc->opcode, 0x02, regD, regD, REGISTER_ZERO, 0);
        break;
      case OPERAND_TYPE_MEMDIR_SYM: 
        insertGenericInstruction(assembler, desc->opcode, 0x02, regD, REGISTER_PC, REGISTER_ZERO, 0);
        insertGenericInstruction(assembler, desc->opcode, 0x02, regD, regD, REGISTER_ZERO, 0);
        break;
      case OPERAND_TYPE_REGDIR:
        insertGenericInstruction(assembler, desc->opcode, 0x01, regD, operand.reg, REGISTER_ZERO, 0);
        break;
      case OPERAND_TYPE_REGIND:
        insertGenericInstruction(assembler, desc->opcode, 0x02, regD, operand.reg, REGISTER_ZERO, 0);
        break;
      case OPERAND_TYPE_REGIND_LIT:
        if(canFitIn12bit(operand.literal)){
          insertGenericInstruction(assembler, desc->opcode, 0x02, regD, operand.reg, REGISTER_ZERO, operand.literal);
        }
        else {
          printf("Literal can't fit 12bit size.\n");
          assembler->correct = false;
        }
        break;
      case OPERAND_TYPE_REGIND_SYM:
        printf("LD REGIND SYM not allowed.\n");
        assembler->correct = false;
        break;
      default:assert(0);
      }
    }
    else if(instrType == INSTR_STR){
      switch(operand.type){
      case OPERAND_TYPE_IMMED_LIT:
      case OPERAND_TYPE_IMMED_SYM:
        printf("STR IMMED SYM not allowed.\n");
        assembler->correct = false;
        break;
      case OPERAND_TYPE_MEMDIR_LIT:
        insertGenericInstruction(assembler, desc->opcode, 0x02, REGISTER_PC, REGISTER_ZERO, regD, 0);
        break;
      case OPERAND_TYPE_MEMDIR_SYM:
        insertGenericInstruction(assembler, desc->opcode, 0x02, REGISTER_PC, REGISTER_ZERO, regD, 0);
        break;
      case OPERAND_TYPE_REGDIR:
        printf("STR REGDIR not allowed\n");
        assembler->correct = false;
        break;
      case OPERAND_TYPE_REGIND:
        insertGenericInstruction(assembler, desc->opcode, 0x00, REGISTER_ZERO, operand.reg, regD, 0);
        break;
      case OPERAND_TYPE_REGIND_LIT:
        if(canFitIn12bit(operand.literal)){
          insertGenericInstruction(assembler, desc->opcode, 0x00, REGISTER_ZERO, operand.reg, regD, operand.literal);
        }
        else {
          printf("Literal can't fit 12bit size.\n");
          assembler->correct = false;
        }
        break;
      case OPERAND_TYPE_REGIND_SYM:
        printf("STR REGIND SYM not allowed.\n");
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
        .reg1 = regD,
      }
    };
    VecLinePush(&current_section->lines,line);
  }
  else {
    printf("No sections defined.\n");
    assembler->correct = false;
  }
}

void instructionCSRReadWrite(struct Assembler *assembler, InstrType instr_type, int regGPR, int regCSR){
  const InstrDesc *desc = instr_descs+instr_type;
  assert(desc->family == INSTR_FAMILY_CSRRD || desc->family == INSTR_FAMILY_CSRWR);

  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];

    if(instr_type == INSTR_CSRRD){
      insertGenericInstruction(assembler, desc->opcode, desc->modifier, regGPR, regCSR, 0, 0);
    }
    else if(instr_type == INSTR_CSRWR){
      insertGenericInstruction(assembler, desc->opcode, desc->modifier, regCSR, regGPR, 0, 0);
    }
    else assert(0);
  }
  else {
    printf("No sections defined.\n");
    assembler->correct = false;
}
}

void instructionJump(struct Assembler *assembler, InstrType instrType, int reg1, int reg2, Operand operand){
  const InstrDesc *desc = instr_descs+instrType;
  assert(desc->family == INSTR_FAMILY_TWOREG_ONEOP);

  if(assembler->sections.size > 0){
    Section* current_section = &assembler->sections.data[assembler->sections.size - 1];

    //litpool 
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
    default:
      assert(0);
    }

    //insert code
    insertGenericInstruction(assembler, desc->opcode, desc->modifier, REGISTER_PC, reg1, reg2, 0);

    Line line ={
      .type = LINE_TYPE_INSTRUCITON,
      .instruction = {
        .type = instrType,
        .operand = operand,
        .reg1 = reg1,
        .reg2 = reg2,
      }
    };
    VecLinePush(&current_section->lines,line);
  }else{
    printf("No sections defined.\n");
    assembler->correct = false;
  }
}

struct Assembler
assemblerCreate(void){
  struct Assembler assembler = (struct Assembler){
    .sections = VecSectionCreate(),
    .symbolTable = VecSymTblCreate(),
    .equExprs = VecEquExprCreate(),
    .correct = true,
  };

  insertSymSection(&assembler,"*UNDEF*");
  insertSymSection(&assembler,"*ABS*");
  
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
  VecEquExprDestroy(&assembler->equExprs);
}



static void exprPrint(const Expression* expr){
  switch(expr->type){
  case EXPR_TYPE_NUMBER:
    printf("%d", expr->val);
    break;

  case EXPR_TYPE_SYMBOL:
    printf("%s", expr->name);
    break;

  case EXPR_TYPE_ADD:
    exprPrint(expr->op1);
    printf(" + ");
    exprPrint(expr->op2);
    break;

  case EXPR_TYPE_SUB:
    exprPrint(expr->op1);
    printf(" - ");
    exprPrint(expr->op2);
    break;

  default: assert(0);
  }
}

static void operandPrint(const Operand *operand){
  switch(operand->type){
  case OPERAND_TYPE_IMMED_LIT:
    printf("<immed>%d", operand->literal);
    break;
  case OPERAND_TYPE_IMMED_SYM:
    printf("<immed>%s", operand->symbol);
    break;
  case OPERAND_TYPE_MEMDIR_LIT:
    printf("<memdir>%d", operand->literal);
    break;
  case OPERAND_TYPE_MEMDIR_SYM:
    printf("<memdir>%s", operand->symbol);
    break;
  case OPERAND_TYPE_REGDIR:
    printf("<regdir>%d", operand->reg);
    break;
  case OPERAND_TYPE_REGIND:
    printf("<regind>[%d]", operand->reg);
    break;
  case OPERAND_TYPE_REGIND_LIT:
    printf("<regind>[%d+%d]", operand->reg, operand->literal);
    break;
  case OPERAND_TYPE_REGIND_SYM:
    printf("<regind>[%d+%s]", operand->reg, operand->symbol);
    break;
  default: assert(0);
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
    case INSTR_FAMILY_ONEREG:
      printf("%%r%d", line->instruction.reg1);
      break;
    case INSTR_FAMILY_TWOREG:
      printf("%%r%d, %%r%d", line->instruction.reg1, line->instruction.reg2);
      break;
    case INSTR_FAMILY_TWOREG_ONEOP:
      printf("%%r%d, %%r%d, ", line->instruction.reg1, line->instruction.reg2);
      operandPrint(&line->instruction.operand);
      break;
    case INSTR_FAMILY_LD:
      operandPrint(&line->instruction.operand);
      printf(", %%r%d", line->instruction.reg1);
      break;
    case INSTR_FAMILY_STR:
      printf("%%r%d, ", line->instruction.reg1);
      operandPrint(&line->instruction.operand);
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
    printf("Section size = %lu\n", section->machineCode.size);

    size_t n_print = 0;

    for(size_t j = 0; j < section->machineCode.size; j++){
      printf("%02x", section->machineCode.data[j]);
      n_print += 1;
      if(n_print % 8 == 0) printf("\n");
      else printf(" ");
    }
    /*
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
    }*/
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

bool exprResolvable(struct Assembler *assembler, Expression* expr){
  switch (expr->type)
  {
  case EXPR_TYPE_NUMBER:
    return true;
    break;
  case EXPR_TYPE_SYMBOL:{
    SymTableRow *sym = SymTableFind(assembler, expr->name);
    return sym != NULL && sym->defined;
  }
  break;
  case EXPR_TYPE_ADD:
  case EXPR_TYPE_SUB:
    return exprResolvable(assembler, expr->op1) && exprResolvable(assembler,expr->op2);
    break;
  default: assert(0);
  }
}

void exprResolve(struct Assembler *assembler, Expression* expr, int* sectionCounts, CORE_ADDR* offset, bool isAdd){
  switch (expr->type) {

  case EXPR_TYPE_NUMBER:
    *offset  += isAdd ? +expr->val : -expr->val;
    break;

  case EXPR_TYPE_SYMBOL:{
    SymTableRow *sym = SymTableFind(assembler, expr->name);
    assert(sym != NULL && sym->defined);
    switch (sym->section){
    case EXTERN_SECTION:
      sectionCounts[sym - assembler->symbolTable.data] += isAdd ? +1 : -1;
      break;
    case ABS_SECTION:
      break;
    default:
      sectionCounts[sym->section] += isAdd ? +1 : -1;
      break;
    }
    *offset  += isAdd ? +sym->value : -sym->value;
  } break;
  
  case EXPR_TYPE_ADD:
    exprResolve(assembler, expr->op1, sectionCounts, offset, isAdd);
    exprResolve(assembler, expr->op2, sectionCounts, offset, isAdd);
    break;

  case EXPR_TYPE_SUB:
    exprResolve(assembler, expr->op1, sectionCounts, offset, isAdd);
    exprResolve(assembler, expr->op2, sectionCounts, offset, !isAdd);
    break;

  default:
    break;
  }
}

void AssemblerEndOfFile(struct Assembler *assembler){
  //equ resolution

  while(true){
    bool anyResolved = false; // did we resolve any equ expr in this iteration
    bool anyUnresolved = false; // is there any equ expr that is still not resolved after this iteration

    for(size_t i = 0; i < assembler->equExprs.size ; i++){
      EquExpr *curr_equ_expr = &assembler->equExprs.data[i];
      if(!curr_equ_expr->resolved) anyUnresolved = true;
      else continue;

      if(!exprResolvable(assembler,&curr_equ_expr->value)) continue;

      int *sectionCounts = myMalloc(sizeof(*sectionCounts) * assembler->symbolTable.size);
      memset(sectionCounts,0,sizeof(*sectionCounts) * assembler->symbolTable.size);
      CORE_ADDR offset = 0;

      exprResolve(assembler,&curr_equ_expr->value, sectionCounts, &offset, true);
      size_t target_section = ABS_SECTION;
      bool error = false;
      for(size_t j = 0; j < assembler->symbolTable.size; j++){
        if(sectionCounts[j] == 0);
        else if(sectionCounts[j]==1){
          if(target_section == ABS_SECTION) target_section = j;
          else {
            error = true;
            break;
          }
        }
        else{
          error = true;
          break;
        }
      }
      if(error){
        printf("Symbol %s cannot be defined by EQU.\n",curr_equ_expr->name);
      }
      else{
        insertSym(assembler, curr_equ_expr->name, target_section, offset);
        curr_equ_expr->resolved = true;
        anyResolved = true;
      }
      myFree(sectionCounts);
    }

    if(!anyUnresolved) break;
    if(!anyResolved){
      printf("Semantic error:Circular dependencies. \n");
      assembler->correct = false;
      return;
    }
  }
  
  //backpatching

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
          printf("Relocation error:forward ref is not a symbol.\n");
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
          printf("Literal can't fit 12bit size.\n");
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
          printf("Literal can't fit 12bit size.\n");
          assembler->correct = false;
        }

        SymTableRow *symbol = SymTableFind(assembler, currentFR->name);
        if(symbol){
          add_data32_reloc(assembler, currentSection, currentSection->machineCode.size + currentFR->lit_idx * 4, symbol);
        }
        else {
          printf("Relocation error:forward ref is not a symbol.\n");
          assembler->correct = false;
        }
      }
      break;
      default:
        assert(0);
        break;
      }
      
    }

    for(size_t j = 0; j < currentSection->litPool.size; j++){
      const LitPoolEntry *lit = &currentSection->litPool.data[j];
      VecBytePush(&currentSection->machineCode, lit->value >>  0);
      VecBytePush(&currentSection->machineCode, lit->value >>  8);
      VecBytePush(&currentSection->machineCode, lit->value >> 16);
      VecBytePush(&currentSection->machineCode, lit->value >> 24);
    }
    
  }
  
}
void assemblerPrintObjectFile(const struct Assembler *assembler,FILE* file){
  fprintf(file,"%lu",assembler->symbolTable.size);
  for(size_t i = 0; i < assembler->symbolTable.size; i++){
    const SymTableRow *curr_sym = &assembler->symbolTable.data[i];
    fprintf(file,"\n%s %lu %u %u %u %u",
      curr_sym->name,
      curr_sym->section,
      curr_sym->value,
      curr_sym->type,
      curr_sym->defined,
      curr_sym->bind
    );
  }
  fprintf(file,"\n%lu",assembler->sections.size);
    for(size_t i = 0; i < assembler->sections.size; i++){
    const Section *current_section = &assembler->sections.data[i];
    fprintf(file,"\n%lu ",current_section->symtabIndex);

    fprintf(file, "\n%lu ", current_section->machineCode.size);

    for(size_t j = 0; j < current_section->machineCode.size; j++){
      if(j % 8 == 0) fprintf(file, "\n");
      fprintf(file, "%02x ", current_section->machineCode.data[j]);
    }

    fprintf(file, "\n%lu", current_section->relocations.size);
    for(size_t j = 0; j < current_section->relocations.size; j++){
      const Relocation *current_reloc = &current_section->relocations.data[j];

      fprintf(file, "\n%lu %u %lu %u",
        current_reloc->offset,
        current_reloc->type,
        current_reloc->symbolIndex,
        current_reloc->addend
      );
    }

  }

}