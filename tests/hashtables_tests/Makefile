DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
CFLAGS_LIBIBERTY=-DHAVE_MALLOC_H -DHAVE_STDINT_H -DHAVE_STDLIB_H -DHAVE_STRING_H

all: clean hcreate hashlibi

hcreate: hcreate.o
	$(CC) -o hcreate hcreate.o
hcreate.o: hcreate.c
	$(CC) -c hcreate.c $(CFLAGS) $(DEBUG)

hashlibi: hashtable_libiberty.o hashtab.o
	$(CC) -o hashlibi hashtable_libiberty.o hashtab.o
hashtab.o: hashtab.c hashtable_libiberty.c
	$(CC)  -c hashtab.c hashtable_libiberty.c $(CFLAGS) $(CFLAGS_LIBIBERTY) $(DEBUG)

clean:
	rm -rf *.o hcreate hashlibi
