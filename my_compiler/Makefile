STD = -std=c++17
CXX = g++ $(STD)
all: compiler

compiler: lexer parser


lexer:
	flex -o lexer.cpp lexer.lex

bison:
	bison -o grammar.cpp -d grammar.y

parser: bison lexer
	$(CXX) -c lexer.cpp -o lexer.o
	$(CXX) -c grammar.cpp -o grammar.o

clean-all:
	rm -f grammar.cpp grammar.hpp lexer.cpp lexer.o grammar.o