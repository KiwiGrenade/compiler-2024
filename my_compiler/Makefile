STD = -std=c++17
CXX = g++ $(STD)

all: compiler

compiler: parser
	$(CXX) lexer.o grammar.o -o compiler

lexer:
	flex -o lexer.cpp lexer.lex

bison:
	bison -o grammar.cpp -d grammar.y

parser: bison lexer
	$(CXX) -c lexer.cpp -o lexer.o
	$(CXX) -c grammar.cpp -o grammar.o

clean-output:
	rm -rf output

clean-all: clean-output
	rm -f grammar.cpp grammar.hpp lexer.cpp lexer.o grammar.o compiler