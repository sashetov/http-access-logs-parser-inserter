DEBUG=-g
#CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS= -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer

all: clean stringmatrix
stringmatrix: 2dstrings.o
	$(CC) -o stringmatrix 2dstrings.o
2dstrings.o:
	$(CC) -c 2dstrings.c $(CFLAGS) $(DEBUG)
clean:
	rm -rf *.o stringmatrix
