STD = -std=c++17
CXX = g++ $(STD)

all: compiler

compiler: parser types
	$(CXX) lexer.o grammar.o AST.o handlers.o  -o compiler

lexer:
	flex -o lexer.cpp lexer.lex

bison:
	bison -o grammar.cpp -d grammar.y

parser: bison lexer
	$(CXX) -c lexer.cpp -o lexer.o
	$(CXX) -c grammar.cpp -o grammar.o
	$(CXX) -c handlers.cpp -o handlers.o

types:
	$(CXX) -c AST.cpp -o AST.o

clean:
	rm -f grammar.cpp grammar.hpp lexer.cpp lexer.o grammar.o handlers.o AST.o compiler