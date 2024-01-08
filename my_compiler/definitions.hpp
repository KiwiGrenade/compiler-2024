#ifndef DEFS_HPP
#define DEFS_HPP
#include <string>
#include <iostream>
#include <memory>
#include "CodeBlock.hpp"
#include "grammar.hpp"
#define ptr(type) std::shared_ptr<type>
#define new_ptr(type, ...) std::make_shared<type>(__VA_ARGS__)

extern int yylex();
extern "C" int yywrap();
void yyerror(const char* string);
int yylex();
int yyparse();

typedef std::string ident;

// void chuj();
std::string handleCondition(std::string VAL1, operator_type OP_TYPE, std::string VAL2);
#define handleExpression handleCondition
extern size_t currLine;

#endif