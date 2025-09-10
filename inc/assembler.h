#ifndef _ASEMBLER_H
#define _ASEMBLER_H

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "vector.h"
#include "instr.h"

VECTOR_DECLARE(VecString, char*);

typedef enum{
  SYM_TBL_TYPE_NOTYPE,
  SYM_TBL_TYPE_FILE_T,
  SYM_TBL_TYPE_SECTION,
  SYM_TBL_TYPE_OBJECT,
  SYM_TBL_TYPE_FUNCTION,

  SYM_TBL_TYPE_COUNT,
}symTableType;

typedef enum{
  FORWARD_REF_DATA32,
  FORWARD_REF_LITPOOL_NUM, // this refers to literal with index lit_idx
  FORWARD_REF_LITPOOL_SYM, // this refers to literal with index lit_idx

  FORWARD_REF_TYPE_COUNT,

}ForwardRefType;

typedef struct ForwardRef{
  size_t offset;
  ForwardRefType type;
  const char *name;
  int addend;
  size_t lit_idx;
} ForwardRef;

VECTOR_DECLARE(VecForwardRef, ForwardRef);

typedef enum RelocationType{
  RELOCATION_TYPE_DATA32,

  RELOACTION_TYPE_COUNT,
}RelocationType;

typedef struct Relocation{
  size_t offset;
  ForwardRefType type;
  size_t symbolIndex;
  int addend;
} Relocation;

VECTOR_DECLARE(VecRelocation, Relocation);

typedef enum{
  BIND_TYPE_LOCAL,
  BIND_TYPE_GLOBAL,

  BIND_TYPE_COUNT,
}symTableBind;

typedef uint32_t CORE_ADDR;

typedef struct SymTableRow{
  char* name;
  size_t section;
  CORE_ADDR value;
  symTableType type;
  bool defined;
  symTableBind bind;
  struct SymTableRow* definition; //not used during assembly phase
} 
SymTableRow;

VECTOR_DECLARE(VecSymTbl,SymTableRow);

typedef enum ExprType{
  EXPR_TYPE_SYMBOL,
  EXPR_TYPE_NUMBER,
  EXPR_TYPE_ADD,
  EXPR_TYPE_SUB
} ExpressionType;

typedef struct Expression{
  ExpressionType type;
  CORE_ADDR val;
  char* name;
  struct Expression *op1, *op2;
} Expression;

VECTOR_DECLARE(VecExpr, Expression);

typedef enum LineType {
  LINE_TYPE_DIRECTIVE,
  LINE_TYPE_INSTRUCITON,
} LineType;

typedef struct Line{
  LineType type;
  struct Directive {
    enum DirectiveType {
      DIRECTIVE_TYPE_WORD,
      DIRECTIVE_TYPE_SKIP,
      DIRECTIVE_TYPE_ASCII,
    }type;

    VecExpr expressions;
    char *string;

  } directive;
  Instruction instruction;
}Line;

typedef struct {
  char* name;
  Expression value;
  bool resolved;
} EquExpr;

VECTOR_DECLARE(VecEquExpr, EquExpr);

VECTOR_DECLARE(VecLine, Line);

VECTOR_DECLARE(VecByte, unsigned char);

typedef struct LitPoolEntry {
  int value;
} LitPoolEntry;

VECTOR_DECLARE(VecLitPoolEntry, LitPoolEntry);

typedef struct Section
{
  size_t symtabIndex;
  VecByte machineCode;
  VecLine lines;

  VecLitPoolEntry litPool;

  VecForwardRef forwardRefs;
  VecRelocation relocations;
}Section;

VECTOR_DECLARE(VecSection, Section);

struct Assembler {
  VecSection sections;
  VecSymTbl symbolTable;
  VecEquExpr equExprs;
  bool correct;

};

#include "parser.tab.h"

#define EXTERN_SECTION 0
#define ABS_SECTION 1

struct Assembler assemblerCreate(void);
void assemblerDestroy(struct Assembler *assembler);
void assemblerPrint(const struct Assembler* assembler);
void AssemblerEndOfFile(struct Assembler *assembler);
void assemblerPrintObjectFile(const struct Assembler *assembler,FILE* file);

//_________________________________________misc_functions________________________________________________
void insertSymSection(struct Assembler* assembler, char* name);
void insertSymLabel(struct Assembler* assembler, char* name);
void insertSymExtern(struct Assembler* assembler, char *name);
void declareSymGlobal(struct Assembler* assembler, char *name);

void printSymTable(const struct Assembler* assembler);
void initSymbolTable(struct Assembler* assembler);

//_________________________________________directives____________________________________________________
void section(struct Assembler* assembler, char* symbol);
void global(struct Assembler* assembler, VecString symlist);
void type(struct Assembler* assembler, char* sym, symTableType type);
void word(struct Assembler* assembler, VecExpr expresions);
void ascii(struct Assembler* assembler, char* string);
void externSym(struct Assembler* assembler, VecString symlist);
void equ(struct Assembler* assembler, char* name, Expression expr);

//_________________________________________instructions____________________________________________________
void instructionNoop(struct Assembler *assembler, InstrType instr_type);
void instructionRet(struct Assembler *assembler);
void instructionIret(struct Assembler *assembler);
void instructionOnereg(struct Assembler *assembler, InstrType instr_type, int reg);
void instructionTworeg(struct Assembler *assembler, InstrType instr_type, int regS, int regD);
void instructionLoadStore(struct Assembler *assembler, InstrType instrType, Operand operand, int regD);
void instructionCSRReadWrite(struct Assembler *assembler, InstrType instr_type, int regGPR, int regCSR);
void instructionJump(struct Assembler *assembler, InstrType instrType, int reg1, int reg2, Operand operand);

#endif