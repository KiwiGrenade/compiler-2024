%{
#include "lexer_pre.hpp"
/*
%token SEMICOLON
%token EQUAL
%token ASSIGN

%token PLUS
%token MINUS
%token ASTERISK
%token SLASH
%token PERCENT

%token DEQUAL
%token NEQUAL
%token LESS
%token MORE
%token LESSEQUAL
%token MOREEQUAL

%token COMMENT

// keywords:
%token KW_CONST
%token KW_VAR
%token KW_BEGIN
%token KW_END
%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_WHILE
%token KW_DO
%token KW_READ
%token KW_WRITE

%token NUM
%token IDENTIFIER
*/
%}

%%
\;              { return SEMICOLON; }
=               { return EQUAL; }
:=              { return ASSIGN; }

\+              { return PLUS; }
-               { return MINUS; }
\*              { return ASTERISK; }
\/              { return SLASH; }
"%"             { return PERCENT; }

==              { return DEQUAL; }
!=              { return NEQUAL; }
"<"             { return LESS; }
">"             { return MORE; }
"<="            { return LESSEQUAL; }
">="            { return MOREEQUAL; }

\(\*([^*]|(\*+([^*)])))*\*+\)             { HANDLE_COMMENT; }

[A-Z]+			{
					std::string KW(yytext);
					     if(KW == "CONST") return KW_CONST;
					else if(KW == "VAR")   return KW_VAR;
					else if(KW == "BEGIN") return KW_BEGIN;
					else if(KW == "END")   return KW_END;
					else if(KW == "IF")    return KW_IF;
					else if(KW == "THEN")  return KW_THEN;
					else if(KW == "ELSE")  return KW_ELSE;
					else if(KW == "WHILE") return KW_WHILE;
					else if(KW == "DO")    return KW_DO;
					else if(KW == "READ")  return KW_READ;
					else if(KW == "WRITE") return KW_WRITE;
					else ERROR_INVALID_KEYWORD;
				}

[[:digit:]]+    { yylval = std::string(yytext); return NUM;}
[_a-z]+         { yylval = std::string(yytext); return IDENTIFIER;}

\r|\r\n|\n		{ HANDLE_NEWLINE; }
[[:space:]]     { /*ignore whitespaces*/ }
.				{ ERROR_INVALID_CHARACTER; }

%%

#include "lexer_post.hpp"
