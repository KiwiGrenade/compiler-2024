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

#ifndef YY_YY_GRAMMAR_HPP_INCLUDED
# define YY_YY_GRAMMAR_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "grammar.y"

    #include "definitions.hpp"
    #include "types.hpp"
    extern FILE* yyin;

#line 55 "grammar.hpp"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMICOLON = 258,               /* SEMICOLON  */
    COMMA = 259,                   /* COMMA  */
    LPRNT = 260,                   /* LPRNT  */
    RPRNT = 261,                   /* RPRNT  */
    LBRCKT = 262,                  /* LBRCKT  */
    RBRCKT = 263,                  /* RBRCKT  */
    ASSIGN = 264,                  /* ASSIGN  */
    PLUS = 265,                    /* PLUS  */
    MINUS = 266,                   /* MINUS  */
    ASTERISK = 267,                /* ASTERISK  */
    FWSLASH = 268,                 /* FWSLASH  */
    PERCENT = 269,                 /* PERCENT  */
    EQUAL = 270,                   /* EQUAL  */
    NEQUAL = 271,                  /* NEQUAL  */
    MORE = 272,                    /* MORE  */
    LESS = 273,                    /* LESS  */
    MOREOREQUAL = 274,             /* MOREOREQUAL  */
    LESSOREQUAL = 275,             /* LESSOREQUAL  */
    KW_PROCEDURE = 276,            /* KW_PROCEDURE  */
    KW_IS = 277,                   /* KW_IS  */
    KW_IN = 278,                   /* KW_IN  */
    KW_END = 279,                  /* KW_END  */
    KW_PROGRAM = 280,              /* KW_PROGRAM  */
    KW_IF = 281,                   /* KW_IF  */
    KW_THEN = 282,                 /* KW_THEN  */
    KW_ELSE = 283,                 /* KW_ELSE  */
    KW_ENDIF = 284,                /* KW_ENDIF  */
    KW_WHILE = 285,                /* KW_WHILE  */
    KW_DO = 286,                   /* KW_DO  */
    KW_ENDWHILE = 287,             /* KW_ENDWHILE  */
    KW_REPEAT = 288,               /* KW_REPEAT  */
    KW_UNTIL = 289,                /* KW_UNTIL  */
    KW_READ = 290,                 /* KW_READ  */
    KW_WRITE = 291,                /* KW_WRITE  */
    KW_T = 292,                    /* KW_T  */
    num = 293,                     /* num  */
    pidentifier = 294              /* pidentifier  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef ident YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_GRAMMAR_HPP_INCLUDED  */
