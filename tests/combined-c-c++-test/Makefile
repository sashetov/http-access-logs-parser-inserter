DEBUG=-g
CXXFLAGS= -Wall -Werror -fPIC -g -O3 -fno-strict-aliasing -fno-omit-frame-pointer -v
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
all: clean square
square: square.o
	$(CXX) $(CXXFLAGS) -o square square.cpp square.o
square.o:
	$(CC) $(CFLAGS) -o square.o -c square.c
clean:
	rm -f *.o square
