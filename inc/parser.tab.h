/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_INC_PARSER_TAB_H_INCLUDED
# define YY_YY_INC_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 16 "misc/parser.y"

  #include "assembler.h"
  #include "vector.h"

#line 54 "inc/parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    GLOBAL = 258,                  /* GLOBAL  */
    EXTERN = 259,                  /* EXTERN  */
    ENDL = 260,                    /* ENDL  */
    SECTION = 261,                 /* SECTION  */
    COLON = 262,                   /* COLON  */
    WORD = 263,                    /* WORD  */
    SKIP = 264,                    /* SKIP  */
    ASCII = 265,                   /* ASCII  */
    EQU = 266,                     /* EQU  */
    END = 267,                     /* END  */
    HALT = 268,                    /* HALT  */
    INT = 269,                     /* INT  */
    IRET = 270,                    /* IRET  */
    CALL = 271,                    /* CALL  */
    RET = 272,                     /* RET  */
    JMP = 273,                     /* JMP  */
    BEQ = 274,                     /* BEQ  */
    BNE = 275,                     /* BNE  */
    BGT = 276,                     /* BGT  */
    PUSH = 277,                    /* PUSH  */
    POP = 278,                     /* POP  */
    XCHG = 279,                    /* XCHG  */
    ADD = 280,                     /* ADD  */
    SUB = 281,                     /* SUB  */
    MUL = 282,                     /* MUL  */
    DIV = 283,                     /* DIV  */
    NOT = 284,                     /* NOT  */
    AND = 285,                     /* AND  */
    OR = 286,                      /* OR  */
    XOR = 287,                     /* XOR  */
    SHL = 288,                     /* SHL  */
    SHR = 289,                     /* SHR  */
    LD = 290,                      /* LD  */
    ST = 291,                      /* ST  */
    CSRRD = 292,                   /* CSRRD  */
    CSRWR = 293,                   /* CSRWR  */
    SYMBOL = 294,                  /* SYMBOL  */
    STRING = 295,                  /* STRING  */
    NUM = 296,                     /* NUM  */
    REG = 297,                     /* REG  */
    SREG = 298                     /* SREG  */
  };
  typedef enum yytokentype yytoken_kind_t;
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
  Operand operand;

#line 123 "inc/parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_INC_PARSER_TAB_H_INCLUDED  */
