CFLAGS 	 += -O3
CXXFLAGS += -std=c++0x

CC        = gcc $(CFLAGS)
CXX       = g++ $(CFLAGS) $(CXXFLAGS)

LDFLAGS  +=


all: compiler


bison:
	bison -o grammar.cpp -d grammar.y	|| echo "No bison found/translate error, using precompiled version"

flex:
	flex -o lexer.cpp lexer.lex 		|| echo "No flex found/translate error, using precompiled version"

parser: flex bison
	$(CXX) -c lexer.cpp -o lexer.o
	$(CXX) -c grammar.cpp -o grammar.o
	$(CXX) -c handlers.cpp -o handlers.o

typeset:
	$(CXX) -c instruction.cpp -o instruction.o
	$(CXX) -c label.cpp -o label.o
	$(CXX) -c var_block.cpp -o var_block.o
	$(CXX) -c snippet.cpp -o snippet.o

optimizer:

utils:
	$(CXX) -c utils.cpp -o utils.o

compiler: parser typeset optimizer utils
	$(CXX) $(LDFLAGS) lexer.o grammar.o instruction.o snippet.o utils.o var_block.o label.o handlers.o -o ./compiler

machine:
	$(CXX) ./interpreter/interpreter.cc -o ./vm

test: all
	./compiler < test.imp > out.asm

clean:
	rm -f *.o 
	rm -f *.asm