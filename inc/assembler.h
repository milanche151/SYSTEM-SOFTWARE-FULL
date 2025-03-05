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
  NOTYPE,
  FILE_T,
  SECTION,
  OBJECT,
  FUNCTION
}symTableType;

typedef enum{
  LOCAL,
  GLOBAL
}symTableBind;

typedef struct ST_forwardrefs{
    uint32_t patch;                       
    struct ST_forwardrefs *nlink;// 
}ST_forwardrefs;

typedef struct SymTableRow{
 int num;
 char* name;
 uint32_t value;
 symTableType type;
 uint32_t offset;
 bool defined;
 symTableBind bind;
 char* ndx;
 ST_forwardrefs *flink;
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

typedef struct Section
{
  char* name;
  uint32_t locationCounter;
  VecLine lines;
}Section;

VECTOR_DECLARE(VecSection, Section);

struct Assembler {
  VecSection sections;
  VecSymTbl symbolTable;
  bool correct;
};

struct Assembler assemblerCreate(void);

void assemblerDestroy(struct Assembler *assembler);

//_________________________________________misc_functions________________________________________________
SymTableRow* createSymSection(struct Assembler* assembler, char* symbol,symTableBind bind);
SymTableRow* createSymbol(struct Assembler* assembler, char* symbol,symTableBind bind,bool defined);
void inserIntoSymbolTable(struct Assembler* assembler, SymTableRow *sym);
void printSymTable(struct Assembler* assembler);
void initSymbolTable(struct Assembler* assembler);

//_________________________________________directives____________________________________________________
void global(struct Assembler* assembler, VecString symlist);
void word(struct Assembler* assembler, VecExpr expresions);
void ascii(struct Assembler* assembler, char* string);
void externSym(struct Assembler* assembler, VecString symlist);


#endif