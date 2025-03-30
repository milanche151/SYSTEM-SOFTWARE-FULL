#include<stdio.h>

#include "assembler.h"

int main(){
  initSymbolTable();
  global("section1 section2");
  printSymTable();
}