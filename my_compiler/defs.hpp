#ifndef DEFS_HPP
#define DEFS_HPP
    #include <string>
    #include <iostream>

    extern int yylex();
    extern "C" int yywrap();

    void yyerror(const char* string);
    int yylex();
    int yyparse();
    extern size_t currLine;

#endif