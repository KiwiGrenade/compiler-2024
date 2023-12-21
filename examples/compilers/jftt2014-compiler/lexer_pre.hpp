#ifndef LEXER_PRE_HPP
#define LEXER_PRE_HPP

#include "types.hpp"
#include "definitions.hpp"

#include "grammar.hpp"

#define HANDLE_COMMENT \
{\
	std::string CC(yytext);\
	int t=0, n=0, r=0;\
	while(t = CC.find('\n', t) != std::string::npos) ++n; t = 0;\
	while(t = CC.find('\r', t) != std::string::npos) ++r;\
	while(n-- > 0 || r-- > 0) HANDLE_NEWLINE;\
}

#define HANDLE_NEWLINE { currentLine++; }

#define ERROR_INVALID_KEYWORD { }
#define ERROR_INVALID_CHARACTER { }



#endif
