DEBUG=-g
CFLAGS=-O2 -Wall -W -fno-strict-aliasing -fno-omit-frame-pointer -v
HASHTAB_CFLAGS=-DHAVE_MALLOC_H -DHAVE_STDINT_H -DHAVE_STDLIB_H -DHAVE_STRING_H 
all: DUMP_CPP_VARS clean test_progs htlog_analyzer

htlog_analyzer: clean_local htlog_analyzer.o
	$(CC) -o htlog_analyzer  hashtab.o ht_nodes.o htlog_processing.o htlog_analyzer.o
htlog_analyzer.o: hashtab.c ht_nodes.c htlog_processing.c htlog_analyzer.c
	$(CC) -c   $(HASHTAB_CFLAGS) $(CFLAGS) $(DEBUG) hashtab.c ht_nodes.c htlog_processing.c htlog_analyzer.c

test_progs:
	$(MAKE) -C tests/ ALL

DUMP_CPP_VARS:
	gcc -dM -E - < /dev/null

clean_local:
	rm -rf *.o htlog_analyzer

clean: clean_local
	$(MAKE) -C tests/ clean
