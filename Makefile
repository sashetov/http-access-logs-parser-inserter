DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer
all: clean test_progs htlog_analyzer

test_progs:
	$(MAKE) -C tests/ all

htlog_analyzer.o: htlog_analyzer.c
	$(CC) -c htlog_analyzer.c $(CFLAGS) $(DEBUG)
htlog_analyzer: htlog_analyzer.o
	$(CC) -o htlog_analyzer htlog_analyzer.o

clean:
	rm -rf *.o htlog_analyzer
	$(MAKE) -C tests/ clean
