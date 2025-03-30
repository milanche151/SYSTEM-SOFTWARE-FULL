#ifndef _ASEMBLER_H
#define _ASEMBLER_H

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "vector.h"

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
  //FORWARD_REF_PC12,

  FORWARD_REF_TYPE_COUNT,

}ForwardRefType;

typedef struct ForwardRef{
  size_t offset;
  ForwardRefType type;
  const char *name;
  int addend;
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
  BIND_TYPE_GLOBAL
}symTableBind;

typedef struct SymTableRow{
 char* name;
 size_t section;
 uint32_t value;
 symTableType type;
 bool defined;
 symTableBind bind;
} SymTableRow;

VECTOR_DECLARE(VecSymTbl,SymTableRow);

typedef enum ExprType{
  EXPR_TYPE_SYMBOL,
  EXPR_TYPE_NUMBER,
} ExpressionType;

typedef struct Expression{
  ExpressionType type;
  uint32_t val;
  char* name;
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
}Line;

VECTOR_DECLARE(VecLine, Line);

VECTOR_DECLARE(VecByte, unsigned char);

typedef struct Section
{
  size_t symtabIndex;
  VecByte machineCode;
  VecLine lines;

  VecForwardRef forwardRefs;
  VecRelocation relocations;
}Section;

VECTOR_DECLARE(VecSection, Section);

struct Assembler {
  VecSection sections;
  VecSymTbl symbolTable;
  bool correct;
};

#include "parser.tab.h"

struct Assembler assemblerCreate(void);
void assemblerDestroy(struct Assembler *assembler);
void assemblerPrint(const struct Assembler* assembler);
void AssemblerEndOfFile(struct Assembler *assembler);

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
void word(struct Assembler* assembler, VecExpr expresions);
void ascii(struct Assembler* assembler, char* string);
void externSym(struct Assembler* assembler, VecString symlist);

#endif