DEBUG=-g
CFLAGS=-Wall -fno-strict-aliasing -fno-omit-frame-pointer #-O2
PTHREAD_CFLAGS=-pthread
all: clean pthread_progs
pthread_progs: pthread_read_n_files pthreads_read_lines
sem: sem.o
	$(CC) -o sem_create sem.o sem_create.c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG)
sem.o:
	$(CC) -c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG) sem.c
pthread_read_n_files: pthreads_read_n_files.c
	$(CC) -o pthread_read_n_files pthreads_read_n_files.c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG)
pthreads_read_lines: pthreads_read_lines.c
	$(CC) -o pthreads_read_lines pthreads_read_lines.c $(CFLAGS) $(PTHREAD_CFLAGS) $(DEBUG)
clean:
	rm -rf *.o pthreads_read_lines pthread_read_n_files sem_create
