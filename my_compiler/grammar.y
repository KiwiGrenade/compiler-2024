%{
    #include "defs.hpp"
    // #include "AST.cpp"
    extern FILE* yyin;
%}
%define api.header.include {"grammar.hpp"}
%define api.value.type {std::string}
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
     identifier ASSIGN expression SEMICOLON
    | KW_IF condition KW_THEN commands KW_ELSE commands KW_ENDIF
    | KW_IF condition KW_THEN commands KW_ENDIF
    | KW_WHILE condition KW_DO commands KW_ENDWHILE
    | KW_REPEAT commands KW_UNTIL condition SEMICOLON
    | proc_call SEMICOLON
    | KW_READ identifier SEMICOLON
    | KW_WRITE value SEMICOLON
 
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
    args COMMA pidentifier
    | pidentifier

    /*expression -> 1 OR 2 values*/
expression:
    value
    | value PLUS value
    | value MINUS value
    | value ASTERISK value
    | value FWSLASH value
    | value PERCENT value

    /*condition -> 2 values*/
condition:
    value EQUAL value
    | value NEQUAL value
    | value MORE value
    | value LESS value
    | value MOREOREQUAL value
    | value LESSOREQUAL value

    /*value -> number or identifier( -> variable or table)*/
value:
    num
    | identifier

identifier: 
    /*identifier( -> variable or table)*/
    pidentifier
    | pidentifier LBRCKT num RBRCKT
    | pidentifier LBRCKT pidentifier RBRCKT  

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
