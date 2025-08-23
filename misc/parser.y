%{
#include <stdio.h>
#include <string.h>
#include "util.h"
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
  Expression expr;
  symTableType type;
}

%debug

%token GLOBAL EXTERN ENDL SECTION COLON WORD SKIP ASCII EQU END HALT INT IRET CALL RET JMP BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR SHL SHR LD ST CSRRD CSRWR 
%token FUNC OBJ TYPE
%token<string> SYMBOL
%token<string> STRING
%type<stringvec> SYMLIST
%type<exprvec> EXPR_LIST
%type<expr> equ_expr equ_primary
%token<number> NUM
%token<number> REG
%token<number> SREG
%type<instrType> noop_opcode
%type<instrType> tworeg_opcode
%type<instrType> jmp_opcode
%type<operand> operand
%type<operand> jmp_operand
%type<type> type

%%
program:
  lines { AssemblerEndOfFile(assembler); }
  ;

lines:
  lines ENDL line 
  | lines ENDL
  | line
  | ENDL
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
  TYPE SYMBOL type {
    type(assembler,$2,$3);
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
  EQU SYMBOL ',' equ_expr{
    equ(assembler, $2, $4);
  }
  |
  END {
   
    AssemblerEndOfFile(assembler);
    YYACCEPT;
  }
  ;

type:
  FUNC { $$ = SYM_TBL_TYPE_FUNCTION;}
  |
  OBJ { $$ = SYM_TBL_TYPE_OBJECT; }
  ;
equ_expr
  : equ_expr '+' equ_primary {
    Expression add = {
      .type = EXPR_TYPE_ADD,
      .op1 = myMalloc(sizeof(*add.op1)),
      .op2 = myMalloc(sizeof(*add.op2))      
    };
    *add.op1 = $1;
    *add.op2 = $3;
    $$ = add;
  }
  | equ_expr '-' equ_primary{
    Expression sub = {
      .type = EXPR_TYPE_SUB,
      .op1 = myMalloc(sizeof(*sub.op1)),
      .op2 = myMalloc(sizeof(*sub.op2))      
    };
    *sub.op1 = $1;
    *sub.op2 = $3;
    $$ = sub;
  }
  | equ_primary
;

equ_primary
  : SYMBOL { $$ = (Expression){ .type = EXPR_TYPE_SYMBOL, .name = $1, }; }
  | NUM { $$ = (Expression){ .type = EXPR_TYPE_NUMBER, .val = $1, }; }
  ;

instruction:
  noop_opcode { instructionNoop(assembler, $1); }
  | RET { instructionRet(assembler); }
  | IRET { instructionIret(assembler); }
  | PUSH REG { instructionOnereg(assembler, INSTR_PUSH, $2); }
  | POP REG { instructionOnereg(assembler, INSTR_POP, $2); }
  | NOT REG { instructionOnereg(assembler, INSTR_NOT, $2); }
  | CALL jmp_operand { instructionJump(assembler, INSTR_CALL, 0, 0, $2); }
  | JMP jmp_operand { instructionJump(assembler, INSTR_JMP, 0, 0, $2); }
  | jmp_opcode REG ',' REG ',' jmp_operand { instructionJump(assembler, $1, $2, $4, $6); }
  | tworeg_opcode REG ',' REG { instructionTworeg(assembler, $1, $2, $4); }
  | LD operand ',' REG { instructionLoadStore(assembler, INSTR_LD, $2,$4); }
  | ST REG ',' operand { instructionLoadStore(assembler, INSTR_STR, $4, $2); }
  | CSRRD SREG ',' REG { instructionCSRReadWrite(assembler, INSTR_CSRRD, $4, $2); }
  | CSRWR REG ',' SREG { instructionCSRReadWrite(assembler, INSTR_CSRWR, $2, $4); }
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

jmp_operand :
  NUM { $$ = (Operand){.type = OPERAND_TYPE_IMMED_LIT,.literal = $1}; }
  | SYMBOL { $$ = (Operand){.type = OPERAND_TYPE_IMMED_SYM,.symbol = $1}; }
  ;

noop_opcode:
  HALT { $$ = INSTR_HALT; }
  | INT { $$ = INSTR_INT; }
  ;

tworeg_opcode:
  ADD { $$ = INSTR_ADD; }
  | SUB { $$ = INSTR_SUB; }
  | MUL { $$ = INSTR_MUL; }
  | DIV { $$ = INSTR_DIV; }
  | SHL { $$ = INSTR_SHL; }
  | SHR { $$ = INSTR_SHR; }
  | AND { $$ = INSTR_AND; }
  | XOR { $$ = INSTR_XOR; }
  | OR { $$ = INSTR_OR; }
  ;

jmp_opcode:
  BEQ { $$ = INSTR_BEQ; }
  | BNE { $$ = INSTR_BNE; }
  | BGT { $$ = INSTR_BGT; }
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
  extern int curr_line;
  printf("Error: %s at line %d.\n", s, curr_line);
}

