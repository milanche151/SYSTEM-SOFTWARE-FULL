%{
#include <stdio.h>
#include <string.h>
#include "./inc/assembler.h"
extern int yylex();
extern FILE *yyin;
 
void yyerror(const char *s);

#include "inc/vector.h"

VECTOR_DECLARE(VecString, char*);

%}
%union{
  int number;
  char *string;
  VecString stringvec;
}

%token GLOBAL EXTERN ENDL SECTION COLON WORD SKIP ASCII EQU END HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR VAL REGIND1 REGIND2 PCREL
%token<string> SYMBOL
%type<stringvec> SYMLIST
%token<number> NUM 
%token<string> COMMA
%token<string> REG
%token<string> SREG

%%
program:
lines;
lines:
lines line | line;
line:
label | label directive | directive | ENDL;
directive:
GLOBAL SYMBOLLIST ENDL{
  global($2);
} |
SECTION SYMBOL ENDL{
  symtable* sym = createSymSection($2,SECTION,LOCAL);
  if(sym) inserIntoSymbolTable(sym);

}
GLOBAL SYMLIST ENDL{

};
label:
SYMBOL COLON {
//Asembler::label($1);
} | 
SYMBOL COLON ENDL{
//Asembler::label($1);
};
SYMLIST:
//SYMBOL{$$=$1;}| SYMLIST COMMA SYMBOL{strcpy($$,$1);strcat($$,$2);strcat($$,$3);}
    SYMBOL              { VecStringCreate(&$$); VecStringPush(&$$, $1); }
  | SYMLIST ',' SYMBOL  { $$=$1; VecStringPush(&$$, $3); }
;

%%

void yyerror(const char* s) {
    printf("Error: %s\n", s);
}

