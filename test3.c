#include<stdio.h>
#include "inc/assembler.h"

int main(){
  initSymbolTable();
  global("section1 section2");
  printSymTable();
}