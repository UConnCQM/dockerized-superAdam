all : BuildAndNot

BuildAndNot : BuildAndNot.o
	../boolstuff-0.1.13/libtool  --tag=CXX --mode=link g++ -I../boolstuff-0.1.13/src -g -O2 -Wall -o BuildAndNot BuildAndNot.o ../boolstuff-0.1.13/src/boolstuff/libboolstuff-0.1.la

BuildAndNot.o : BuildAndNot.cpp
	g++ -I. -I../boolstuff-0.1.13/src -g -O2 -Wall -c -o BuildAndNot.o BuildAndNot.cpp

clean:
	rm -rf *.o BuildAndNot
	
