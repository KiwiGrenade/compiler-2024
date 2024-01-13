#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <iostream>
#include <memory>

// extern bool verbose;

#define logme(str, head) { std::cerr << "[LOG][" << head << "]" << str << "\n"; }

typedef std::string ident;
#define ptr(type) std::shared_ptr<type>
#define new_ptr(type, ...) std::make_shared<type>(__VA_ARGS__)

#include "CodeBlock.hpp"
#include "handlers.hpp"
#include "grammar.hpp"

extern "C" int yywrap();
void yyerror(const char* string);
int yylex();
int yyparse();

void error(std::string msg, bool fatal);
void warning(std::string msg);

#include "map"

// void chuj();
extern size_t currLine;

#endif  // DEFS_HPP