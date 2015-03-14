all: compiler

compiler: compiler.cpp parser.o scanner.o token.o
	g++ -std=c++11 -O compiler.cpp parser.o -ocompiler

parser.o: parser.cpp parser.h scanner.o
	g++ -std=c++11 -c -O parser.cpp scanner.o -oparser.o

scanner.o: scanner.cpp scanner.h sym.o
	g++ -std=c++11 -c -O scanner.cpp sym.o -oscanner.o

sym.o: sym.h
	g++ -std=c++11 -c -O sym.h -osym.o

token.o: token.cpp sym.h
	g++ -std=c++11 -c -O scanner.cpp -oscanner.o
