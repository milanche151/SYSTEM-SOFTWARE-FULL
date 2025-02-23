#ifndef _ASEMBLER_H
#define _ASEMBLER_H
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vector.h"

//enums
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
 int defined;
 symTableBind bind;
 char* ndx;
 ST_forwardrefs *flink;
} SymTableRow;
VECTOR_DECLARE(VecSymTbl,SymTableRow);
VECTOR_DECLARE(VecString, char*);
static uint32_t symCounter = 0;
VecSymTbl symbolTable;
uint32_t sectionFinished;
uint32_t programCounter; 

//functions
SymTableRow* createSymSection(char* symbol,symTableBind bind);
SymTableRow* createSymbol(char* symbol,symTableBind bind,int defined);
int inserIntoSymbolTable(SymTableRow *sym);
void printSymTable();
int global(VecString symlist);
int word(VecString symlist);
int externSym(VecString symlist);
void initSymbolTable();


#endif