%{
#include "grammar_pre.hpp"
%}

%define api.value.type {std::string}
%define parse.error verbose

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

%%

program:
    KW_CONST cdeclarations KW_VAR vdeclarations KW_BEGIN commands KW_END { handleProgram($6); };

cdeclarations:
    cdeclarations identifier EQUAL num { $$ = handleCDecl($2, $4); }|
    %empty;

vdeclarations:
    identifier vdeclarations { $$ = handleVDecl($1); } |
    %empty                   { $$ = handleLastDecl();} ;

commands:
    commands command { $$ = handleCommand($1, $2);   } |
    %empty           { $$ = handleEmptyCommand();    };

command:
    identifier ASSIGN expression SEMICOLON                      { $$ = handleAssign($1, $3); } |
    KW_IF condition KW_THEN commands KW_ELSE commands KW_END    { $$ = handleIf($2, $4, $6); } |
    KW_WHILE condition KW_DO commands KW_END                    { $$ = handleWhile($2, $4);  } |
    KW_READ identifier SEMICOLON    { $$ = handleRead($2);  } |
    KW_WRITE identifier SEMICOLON   { $$ = handleWrite($2); } ;

expression:
    num         {$$ = handleExpression($1, NUM,         ""); } |
    identifier  {$$ = handleExpression($1, EQUAL,       "");  checkIdentAtTime($1); } |
    identifier PLUS     identifier {$$ = handleExpression($1, PLUS,     $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier MINUS    identifier {$$ = handleExpression($1, MINUS,    $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier ASTERISK identifier {$$ = handleExpression($1, ASTERISK, $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier SLASH    identifier {$$ = handleExpression($1, SLASH,    $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier PERCENT  identifier {$$ = handleExpression($1, PERCENT,  $3); checkIdentAtTime($1); checkIdentAtTime($3); } ;

condition:
    identifier DEQUAL    identifier { $$ = handleCondition($1, DEQUAL,    $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier NEQUAL    identifier { $$ = handleCondition($1, NEQUAL,    $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier LESS      identifier { $$ = handleCondition($1, LESS,      $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier MORE      identifier { $$ = handleCondition($1, MORE,      $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier LESSEQUAL identifier { $$ = handleCondition($1, LESSEQUAL, $3); checkIdentAtTime($1); checkIdentAtTime($3); } |
    identifier MOREEQUAL identifier { $$ = handleCondition($1, MOREEQUAL, $3); checkIdentAtTime($1); checkIdentAtTime($3); } ;

num:
    NUM;

identifier:
    IDENTIFIER;

%%

#include "grammar_post.hpp"
