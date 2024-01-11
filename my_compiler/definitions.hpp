#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <iostream>
#include <memory>

typedef std::string ident;
#define ptr(type) std::shared_ptr<type>
#define new_ptr(type, ...) std::make_shared<type>(__VA_ARGS__)

#include "CodeBlock.hpp"
#include "handlers.hpp"
#include "grammar.hpp"


extern int yylex();
extern "C" int yywrap();
void yyerror(const char* string);
int yylex();
int yyparse();


// void chuj();
extern size_t currLine;

#endif  // DEFS_HPP