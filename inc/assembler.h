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
static uint32_t symCounter = 0;
VecSymTbl symbolTable;
uint32_t sectionFinished;
uint32_t programCounter; 

//functions
SymTableRow* createSymSection(char* symbol,symTableType type,symTableBind bind);
int inserIntoSymbolTable(SymTableRow *sym);
void printSymTable();
int global(char* symlist);
void initSymbolTable();


#endif