%{
#include <stdio.h>
#include <string.h>

extern int yylex();
extern FILE *yyin;
 
void yyerror(const char *s);

extern struct Assembler *assembler;


%}
%code requires{
  #include "./inc/assembler.h"
  #include "inc/vector.h"
}
%union{
  int number;
  char *string;
  VecString stringvec;
  VecExpr exprvec;
}

%token GLOBAL EXTERN ENDL SECTION COLON WORD SKIP ASCII EQU END HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR VAL REGIND1 REGIND2 PCREL
%token<string> SYMBOL
%type<stringvec> SYMLIST
%type<exprvec> EXPR_LIST
%token<number> NUM 
%token<strin
g> COMMA
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
  SECTION SYMBOL ENDL{
    SymTableRow* sym = createSymSection(assembler,$2,BIND_TYPE_LOCAL);
    if(sym) inserIntoSymbolTable(assembler,sym);
  }
  |
  GLOBAL SYMLIST ENDL{
    global(assembler,$2);
  }
  |
  WORD EXPR_LIST ENDL {
    word(assembler,$2);
  }
  |
  EXTERN SYMLIST ENDL{
    externSym(assembler,$2);
  }
  |
  ASCII SYMBOL ENDL{
    ascii(assembler,SYMBOL);
  };

label:
  SYMBOL ':' ENDL{
    SymTableRow* sym = createSymbol(assembler, $1,SECTION,BIND_TYPE_LOCAL);
    if(sym) inserIntoSymbolTable(assembler, sym);
  };

SYMLIST:
    SYMBOL              { $$ = VecStringCreate(); VecStringPush(&$$, $1); }
  | SYMLIST ',' SYMBOL  { $$=$1; VecStringPush(&$$, $3); };

EXPR_LIST:
    SYMBOL{ $$ = VecExprCreate(); VecExprPush(&$$, (Expression){.type = EXPR_TYPE_SYMBOL,.name = $1});}
  | NUM{$$ = VecExprCreate(); VecExprPush(&$$, (Expression){.type = EXPR_TYPE_SYMBOL,.val = $1});}
  | EXPR_LIST ',' SYMBOL{VecExprPush(&$$, (Expression){.type = EXPR_TYPE_SYMBOL,.name = $3});}
  | EXPR_LIST ',' NUM{VecExprPush(&$$, (Expression){.type = EXPR_TYPE_SYMBOL,.val = $3});}
  ;

%%

void yyerror(const char* s) {
    printf("Error: %s\n", s);
}

