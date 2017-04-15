#!/bin/bash
gcc -c -O2 -Wall -W -g parse.c
gcc -c -O2 -Wall -W -g aht.c
gcc -o parse parse.o aht.o
rm -f *.o
