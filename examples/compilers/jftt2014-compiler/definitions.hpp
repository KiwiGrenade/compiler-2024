#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include<iostream>
#include<memory>

extern bool verbose;
extern bool silent;

extern bool no_sub;
extern bool no_jzero;
extern bool use_temp_dest;

#define logme(a) { if(verbose) std::cerr << "[LOG] " << a << std::endl; }
#define info(a) { if(!silent) std::cerr << "[INFO] " << a << std::endl; }

// Stringifyers
#define S(x) #x
#define SX(x) S(x)

#include "typedefs.hpp"

extern bool parsing;
extern uint32 currentLine;

int yylex();
int yyparse();
extern FILE* yyin;

extern "C" int yywrap();
void yyerror(const char *s);

void error(std::string msg, bool fatal);
void warning(std::string msg);

#include <map>
#include <set>

extern std::map<ident, int> labels;
extern std::map<ident, int> registers;
extern std::map<ident, uint64> constants;
extern std::map<ident, bool> variables;

// Handlers for parser
void startHandling();

ident handleCDecl(ident name, std::string val);
ident handleVDecl(ident name);
ident handleLastDecl();

ident handleAssign(ident name, ident expr);

ident handleEmptyCommand();
ident handleCommand(ident block, ident name);
ident handleProgram(ident block);

ident handleIf(ident cond, ident blockIf, ident blockElse);
ident handleWhile(ident cond, ident block);
ident handleRead(ident var);
ident handleWrite(ident var);

ident handleExpression(ident expr_arg1, int op, ident expr_arg2);
#define handleCondition handleExpression
void checkIdentAtTime(ident id);

// Tokens
#include "grammar.hpp"

#define printset(set) if(verbose) { std::cerr << "{"; for(auto &_ps_s_it : set) { std::cerr << _ps_s_it << " "; } std::cerr << "}\n";}

#include <algorithm>
#include <deque>

// Set helper:
template<typename T>
std::deque<T> multi_to_sorted(std::multiset<T> mset)
{    
    std::set<T> set(mset.begin(), mset.end()); // remove 'multi' from set
    std::deque<T> sorted(set.begin(), set.end());
    
    std::sort(sorted.begin(), sorted.end(), [&](T v1, T v2)
        {
            return mset.count(v1) > mset.count(v2);
        });
    
    return sorted;
}

template<typename K, typename V, typename SRC>
std::multimap<K, V> reverse_map(const SRC& source_map)
{
    std::multimap<K, V> result;
    for(auto &pair : source_map)
    {
        result.emplace(pair.second, pair.first);
    }
    return result;
}

// one-liners:
template<typename T>
inline bool is_power_of_2 (const T &x)
{
  return ((x != 0) && !(x & (x - 1)));
}

#endif
