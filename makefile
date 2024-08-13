all: clean
	flex scanner.lex
	bison -d parser.ypp
	g++ -g -std=c++17 -o hw5.out *.c *.cpp
clean:
	rm -f lex.yy.c
	rm -f parser.tab.*pp
	rm -f hw5.out
.PHONY: all clean