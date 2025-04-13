%{
#include <stdio.h>
#include <string.h>

#include "instr.h"

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
  InstrType instrType;
  Operand operand;
}

%token GLOBAL EXTERN ENDL SECTION COLON WORD SKIP ASCII EQU END HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR
%token<string> SYMBOL
%token<string> STRING
%type<stringvec> SYMLIST
%type<exprvec> EXPR_LIST
%token<number> NUM
%token<number> REG
%token<number> SREG
%type<instrType> noop_opcode
%type<instrType> tworeg_opcode
%type<operand> operand

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
  | label instruction 
  | instruction
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

instruction:
  noop_opcode { instructionNoop(assembler, $1); }
  | tworeg_opcode REG ',' REG { instructionTworeg(assembler, $1, $2, $4); }
  | LD operand ',' REG { instructionLoadStore(assembler, INSTR_LD, $2,$4); }
  | ST REG ',' operand { instructionLoadStore(assembler, INSTR_STR, $4, $2); }
  ;

operand:
  '$' NUM { $$ = (Operand){.type = OPERAND_TYPE_IMMED_LIT,.literal = $2}; }
  | '$' SYMBOL { $$ = (Operand){.type = OPERAND_TYPE_IMMED_SYM,.symbol = $2}; }
  | NUM { $$ = (Operand){.type = OPERAND_TYPE_MEMDIR_LIT,.literal = $1}; }
  | SYMBOL { $$ = (Operand){.type = OPERAND_TYPE_MEMDIR_SYM,.symbol = $1}; }
  | REG { $$ = (Operand){.type = OPERAND_TYPE_REGDIR,.reg = $1}; }
  | '[' REG ']' { $$ = (Operand){.type = OPERAND_TYPE_REGIND,.reg = $2}; }
  | '[' REG '+' NUM ']' { $$ = (Operand){.type = OPERAND_TYPE_REGIND_LIT,.reg = $2,.literal = $4}; }
  | '[' REG '+' SYMBOL ']' { $$ = (Operand){.type = OPERAND_TYPE_REGIND_SYM,.reg = $2,.symbol = $4}; }
  ;

noop_opcode:
  HALT { $$ = INSTR_HALT; }
  ;

tworeg_opcode:
  ADD { $$ = INSTR_ADD; }
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

