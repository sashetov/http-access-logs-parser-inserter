DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
PTHREAD_CFLAGS=-pthread
all: clean pthread_progs

pthread_progs: pthread_read_n_files pthreads_read_lines
pthread_read_n_files: pthreads_read_n_files.c
	$(CC) -o pthread_read_n_files pthreads_read_n_files.c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG)

pthreads_read_lines: pthreads_read_lines.c
	$(CC) -o pthreads_read_lines pthreads_read_lines.c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG)

clean:
	rm -rf *.o pthreads_read_lines pthread_read_n_files
