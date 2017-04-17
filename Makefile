DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
HASHTAB_CFLAGS=-DHAVE_MALLOC_H -DHAVE_STDINT_H -DHAVE_STDLIB_H -DHAVE_STRING_H 
ALL: clean test_progs htlog_analyzer

htlog_analyzer: htlog_analyzer.o hashtab.o
	$(CC) -o htlog_analyzer htlog_analyzer.o hashtab.o
htlog_analyzer.o: htlog_processing.c ht_nodes.c htlog_analyzer.c
	$(CC) -c  $(CFLAGS) $(DEBUG) htlog_processing.c ht_nodes.c htlog_analyzer.c
hashtab.o:  hashtab.c
	$(CC) -c hashtab.c $(HASHTAB_CFLAGS) $(CFLAGS) $(DEBUG)

test_progs:
	$(MAKE) -C tests/ ALL

DUMP_CPP_VARS:
	gcc -dM -E - < /dev/null

clean:
	rm -rf *.o htlog_analyzer
	$(MAKE) -C tests/ clean
