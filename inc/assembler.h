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
  FILE,
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
    ST_forwardrefs *nlink;// 
}ST_forwardrefs;
//structs
typedef struct SymbolTable{
 int num;
 char* name;
 uint32_t value;
 symTableType type;
 uint32_t size;
 int defined;
 symTableBind bind;
 char* section;
 ST_forwardrefs *flink;
} symtable;


//VECTOR_DECLARE(VecSymTbl,symtable);

//functions
symtable* createSymSection(char* symbol,symTableType type,symTableBind);
int inserIntoSymbolTable(symtable* sym);
int global(char* symlist);


#endif