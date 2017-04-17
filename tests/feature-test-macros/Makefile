DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
all: clean ftm

ftm: ftm.o
	$(CC) -o ftm ftm.o
ftm.o: ftm.c
	$(CC) -c ftm.c $(CFLAGS) $(DEBUG)

clean:
	rm -rf *.o ftm
