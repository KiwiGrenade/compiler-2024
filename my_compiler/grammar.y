%token SEMICOLON
%token EQUAL
%token ASSIGN
%token PLUS
%token MINUS
%token ASTERISK
%token FSLASH
%token PERCENT
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

%%

program_all  -> procedures main

procedures   -> procedures KW_PROCEDURE proc_head KW_IS declarations KW_IN commands KW_END
             | procedures KW_PROCEDURE proc_head KW_IS KW_IN commands KW_END
             | 

main         -> KW_PROGRAM KW_IS declarations KW_IN commands KW_END
             | KW_PROGRAM KW_IS KW_IN commands KW_END

commands     -> commands command
             | command

command      -> identifier ASSIGN expression SEMICOLON
             | KW_IF condition KW_THEN commands KW_ELSE commands KW_ENDIF
             | KW_IF condition KW_THEN commands KW_ENDIF
             | KW_WHILE condition KW_DO commands KW_ENDWHILE
             | KW_REPEAT commands KW_UNTIL condition SEMICOLON
             | proc_call SEMICOLON
             | KW_READ identifier SEMICOLON
             | KW_WRITE value SEMICOLON

proc_head    -> pidentifier ( args_decl )

proc_call    -> pidentifier ( args )

declarations -> declarations, pidentifier
             | declarations, pidentifier[num]
             | pidentifier
             | pidentifier[num]

args_decl    -> args_decl, pidentifier
             | args_decl, KW_T pidentifier
             | pidentifier
             | KW_T pidentifier

args         -> args, pidentifier
             | pidentifier

expression   -> value
             | value PLUS value
             | value MINUS value
             | value ASTERISK value
             | value FSLASH value
             | value PERCENT value

condition    -> value EQUAL value
             | value NEQUAL value
             | value MORE value
             | value LESS value
             | value MOREOREQUAL value
             | value LESSOREQUAL value

value        -> num
             | identifier

identifier   -> pidentifier
             | pidentifier[num]
             | pidentifier[pidentifier]
