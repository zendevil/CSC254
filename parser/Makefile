# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

# Note that rule for goal (parse) must be the first one in this file.

# parse: parse.o scan.o
# 	gcc -o -g3 parse parse.o scan.o

# clean:
# 	rm *.o parse

# parse.o: scan.h
# scan.o: scan.h


parse: parse.o scan.o
	g++ -g3 -o  parse parse.o scan.o

clean:
	rm *.o parse

parse.o: scan.h
scan.o: scan.h
