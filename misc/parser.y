%{
#include <stdio.h>
#include <string.h>

extern int yylex();
extern FILE *yyin;
 
void yyerror(const char *s);

extern struct Assembler *assembler;


%}
%code requires{
  #include "assembler.h"
  #include "vector.h"
}
%union{
  int number;
  char *string;
  VecString stringvec;
  VecExpr exprvec;
}

%token GLOBAL EXTERN ENDL SECTION COLON WORD SKIP ASCII EQU END HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR
%token<string> SYMBOL
%token<string> STRING
%type<stringvec> SYMLIST
%type<exprvec> EXPR_LIST
%token<number> NUM 
%token<string> COMMA
%token<string> REG
%token<string> SREG

%%
program:
  lines { AssemblerEndOfFile(assembler); }
  ;

lines:
  lines ENDL line 
  | lines ENDL
  | line
  ;
  
line:
  label
  | label directive 
  | directive
  ;

directive:
  SECTION SYMBOL {
    section(assembler, $2);
  }
  |
  GLOBAL SYMLIST {
    global(assembler,$2);
  }
  |
  WORD EXPR_LIST {
    word(assembler,$2);
  }
  |
  EXTERN SYMLIST {
    externSym(assembler,$2);
  }
  |
  ASCII STRING {
    ascii(assembler,$2);
  }
  |
  END {
   
    AssemblerEndOfFile(assembler);
    YYACCEPT;
  }
  ;

label:
  SYMBOL ':' {
    insertSymLabel(assembler, $1);
  };

SYMLIST:
    SYMBOL              { $$ = VecStringCreate(); VecStringPush(&$$, $1); }
  | SYMLIST ',' SYMBOL  { $$=$1; VecStringPush(&$$, $3); };

EXPR_LIST:
    SYMBOL{ $$ = VecExprCreate(); VecExprPush(&$$, (Expression){.type = EXPR_TYPE_SYMBOL,.name = $1});}
  | NUM{$$ = VecExprCreate(); VecExprPush(&$$, (Expression){.type = EXPR_TYPE_NUMBER,.val = $1});}
  | EXPR_LIST ',' SYMBOL{VecExprPush(&$$, (Expression){.type = EXPR_TYPE_SYMBOL,.name = $3});}
  | EXPR_LIST ',' NUM{VecExprPush(&$$, (Expression){.type = EXPR_TYPE_NUMBER,.val = $3});}
  ;

%%

void yyerror(const char* s) {
    printf("Error: %s\n", s);
}

