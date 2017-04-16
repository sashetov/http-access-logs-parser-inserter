DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
all: clean get_dents move_files

get_dents.o: get_dents.c
	$(CC) -c get_dents.c $(CFLAGS) $(DEBUG)
get_dents: get_dents.o
	$(CC) -o get_dents get_dents.o

move_files.o: move_files.c
	$(CC) -c move_files.c $(CFLAGS) $(DEBUG)
move_files: move_files.o
	$(CC) -o move_files move_files.o

clean:
	rm -rf *.o get_dents move_files
