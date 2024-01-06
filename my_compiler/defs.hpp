#ifndef DEFS_HPP
#define DEFS_HPP
    #include <string>
    #include <iostream>
    #include <memory>

    #define ptr(type) std::shared_ptr<type>
    #define new_ptr(type, ...) std::make_shared<type>(__VA_ARGS__)

    extern int yylex();
    extern "C" int yywrap();
    void yyerror(const char* string);
    int yylex();
    int yyparse();
    
    typedef std::string ident;

    extern size_t currLine;

#endif