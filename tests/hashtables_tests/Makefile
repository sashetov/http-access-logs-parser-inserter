DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
all: clean hcreate

hcreate.o: hcreate.c
	$(CC) -c hcreate.c $(CFLAGS) $(DEBUG)
hcreate: hcreate.o
	$(CC) -o hcreate hcreate.o

clean:
	rm -rf *.o hcreate
