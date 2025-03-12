
%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>
%}
%option noyywrap
%%
[ \t];


.global {return GLOBAL;}
.extern {return EXTERN;}
.section {return SECTION;}
.word {return WORD;}
.skip {return SKIP;}
.equ {return EQU;}
.ascii {return ASCII;}
.end {return END;}
halt {return HALT;}
int {return INT;}
iret {return IRET;}
call {return CALL;}
ret {return RET;}
jmp {return JMP;}
beq {return BEQ;}
bne {return BNE;}
bgt {return BGT;}
push {return PUSH;}
pop {return POP;}
xchg {return XCHG;}
add {return ADD;}
sub {return SUB;}
mul {return MUL;}
div {return DIV;}
not {return NOT;}
and {return AND;}
or {return OR;}
xor {return XOR;}
shl {return SHL;}
shr {return SHR;}
ld {return LD;}
st {return ST;}
csrrd {return CSRRD;}
csrwr {return CSRWR;}
%cause {yylval.number=2;return SREG;}
%handler {yylval.number=1;return SREG;}
%status {yylval.number=0;return SREG;}
% {return PCREL;}
\n {return ENDL;}
[-]?[0-9]+ {yylval.number=atoi(yytext);return NUM;}
: {return COLON;}
, {yylval.string=strdup(yytext);return COMMA;}
[a-zA-Z_][a-zA-Z_0-9]* {yylval.string=strdup(yytext);return SYMBOL;}
\"[a-zA-Z_0-9]*\" { yylval.string=strdup(yytext); return STRING; }
[$] {return VAL;}
[0][x][A-Fa-f0-9]+ {sscanf(yytext,"%x",&yylval.number);return NUM;}
%r[0-15] {yylval.number=atoi(yytext+1);return REG;}
"[" {return REGIND1;}
"]" {return REGIND2;}
[#][^\n]* ;
. {printf("ERROR UNRECOGNIZED ELEMENT");exit(-1);}
%%

