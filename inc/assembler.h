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
  BIND_TYPE_LOCAL,
  BIND_TYPE_GLOBAL
}symTableBind;

typedef struct ForwardRef{
  size_t section;
  uint32_t offset;
  char *name;
} ForwardRef;

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
  int val;
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