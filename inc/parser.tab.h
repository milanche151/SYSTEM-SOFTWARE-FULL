/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_INC_PARSER_TAB_H_INCLUDED
# define YY_YY_INC_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 16 "misc/parser.y"

  #include "assembler.h"
  #include "vector.h"

#line 53 "inc/parser.tab.h"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    GLOBAL = 258,
    EXTERN = 259,
    ENDL = 260,
    SECTION = 261,
    COLON = 262,
    WORD = 263,
    SKIP = 264,
    ASCII = 265,
    EQU = 266,
    END = 267,
    HALT = 268,
    INT = 269,
    IRET = 270,
    CALL = 271,
    RET = 272,
    JMP = 273,
    BEQ = 274,
    BNE = 275,
    BGT = 276,
    PUSH = 277,
    POP = 278,
    XCHG = 279,
    ADD = 280,
    SUB = 281,
    MUL = 282,
    DIV = 283,
    NOT = 284,
    AND = 285,
    OR = 286,
    XOR = 287,
    SHL = 288,
    SHR = 289,
    LD = 290,
    ST = 291,
    CSRRD = 292,
    CSRWR = 293,
    SYMBOL = 294,
    STRING = 295,
    NUM = 296,
    REG = 297,
    SREG = 298
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 20 "misc/parser.y"

  int number;
  char *string;
  VecString stringvec;
  VecExpr exprvec;
  InstrType instrType;

#line 116 "inc/parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INC_PARSER_TAB_H_INCLUDED  */
