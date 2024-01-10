%code requires {
    #include "definitions.hpp"
    #include "types.hpp"
    extern FILE* yyin;
}
/* %define api.header.include {"grammar.hpp"} */
%define api.value.type {ident}
%define parse.error verbose
%locations

// punctuators
%token SEMICOLON
%token COMMA
%token LPRNT
%token RPRNT
%token LBRCKT
%token RBRCKT

// assignment operators
%token ASSIGN

// arithmetic operators
%token PLUS
%token MINUS
%token ASTERISK
%token FWSLASH
%token PERCENT

// relational operators
%token EQUAL
%token NEQUAL
%token MORE
%token LESS
%token MOREOREQUAL
%token LESSOREQUAL

// keywords
%token KW_PROCEDURE
%token KW_IS
%token KW_IN
%token KW_END
%token KW_PROGRAM
%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_ENDIF
%token KW_WHILE
%token KW_DO
%token KW_ENDWHILE
%token KW_REPEAT
%token KW_UNTIL
%token KW_READ
%token KW_WRITE
%token KW_T

// atomic tokens
%token num
%token pidentifier


%%

    /* program all -> procedures + main body */
program_all:
    procedures main

    /* procedures -> 0 or more procedure */
procedures:
     procedures KW_PROCEDURE proc_head KW_IS declarations KW_IN commands KW_END
    | procedures KW_PROCEDURE proc_head KW_IS KW_IN commands KW_END
    | %empty

    /* main - > (var declarations + commands) OR commands */
main:
    KW_PROGRAM KW_IS declarations KW_IN commands KW_END
    | KW_PROGRAM KW_IS KW_IN commands KW_END

    /* commands -> one or more command */
commands:
    commands command
    | command

    /* command -> self explenatory 
    assign,
    if, 
    ifelse, 
    while, 
    repeat, 
    procedure(function)call, 
    read, 
    write) */
command:
     identifier ASSIGN expression SEMICOLON                         {$$ = handleAssignment($1, $3);}
    | KW_IF condition KW_THEN commands KW_ELSE commands KW_ENDIF    {$$ = handleIfElse($2, $4, $6);}
    | KW_IF condition KW_THEN commands KW_ENDIF                     {$$ = handleIf($2, $4);}
    | KW_WHILE condition KW_DO commands KW_ENDWHILE                 {$$ = handleWhile($2, $4);}
    | KW_REPEAT commands KW_UNTIL condition SEMICOLON               {$$ = handleRepeat($2, $4);}
    | proc_call SEMICOLON                                           {$$ = handleProcCall($1);}
    | KW_READ identifier SEMICOLON                                  {$$ = handleRead($2, content_type::_READ, "_READ");}
    | KW_WRITE value SEMICOLON                                      {$$ = handleWrite($2, content_type::_WRITE, "_WRITE");}
 
proc_head:
    pidentifier LPRNT args_decl RPRNT

proc_call: 
    pidentifier LPRNT args RPRNT

    /*declarations -> one ore more ints or tables, separated by commas*/
declarations:
     declarations COMMA pidentifier
    | declarations COMMA pidentifier LBRCKT num RBRCKT
    | pidentifier
    | pidentifier LBRCKT num RBRCKT

    /*args_decl -> one or more ints or tables, separated by commas (with T before tables)*/
args_decl:
     args_decl COMMA pidentifier
    | args_decl COMMA KW_T pidentifier
    | pidentifier
    | KW_T pidentifier
    
    /*args -> one or more ints or tables (without T) */
args:
    args COMMA pidentifier  {$$ = $1 + $2 + $3;}
    | pidentifier           {$$ = $1;}

    /*expression -> 1 OR 2 values*/
expression:
    value                   {$$ = handleExpression($1, operator_type::_NONE, "_NONE");}
    | value PLUS value      {$$ = handleExpression($1, operator_type::_ADD, $3);}
    | value MINUS value     {$$ = handleExpression($1, operator_type::_SUB, $3);}
    | value ASTERISK value  {$$ = handleExpression($1, operator_type::_MUL, $3);}
    | value FWSLASH value   {$$ = handleExpression($1, operator_type::_DIV, $3);}
    | value PERCENT value   {$$ = handleExpression($1, operator_type::_MOD, $3);}

    /*condition -> 2 values*/
condition:
    value EQUAL value           {$$ = handleCondition($1, operator_type::_EQ, $3);}
    | value NEQUAL value        {$$ = handleCondition($1, operator_type::_NEQ, $3);}
    | value MORE value          {$$ = handleCondition($1, operator_type::_LMORE, $3);}
    | value LESS value          {$$ = handleCondition($1, operator_type::_LLESS, $3);}
    | value MOREOREQUAL value   {$$ = handleCondition($1, operator_type::_LHEQ, $3);}
    | value LESSOREQUAL value   {$$ = handleCondition($1, operator_type::_LLEQ, $3);}

    /*value -> number or identifier( -> variable or table)*/
value:
    num             {$$ = $1;}       
    | identifier    {$$ = $1;}

identifier: 
    /*identifier( -> variable or table)*/
    pidentifier                                 {$$ = $1;}
    | pidentifier LBRCKT num RBRCKT             {$$ = $1;}
    | pidentifier LBRCKT pidentifier RBRCKT     {$$ = $1;}

%%
void yyerror(const char *string)
{
    std::cerr << "Error at line " << currLine << ": " << string << std::endl;
}

int yywrap()
{
    return 1;
}

int main(int argc, char* argv[])
{
    FILE *pFILE_IN  = fopen(argv[1], "r");
    FILE *pFILE_OUT = fopen(argv[2], "w");

    if(!pFILE_IN)
    {
        std::cerr << "ERROR: file " << argv[1] << " not found!" << std::endl;
        return 1;
    }

    yyin = pFILE_IN;


    yyparse();
    return 0;
}
