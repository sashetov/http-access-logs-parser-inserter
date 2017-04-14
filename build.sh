#!/bin/bash
gcc -pthread -o pthread_read_lines pthreads_read_lines.c
#gcc -pthread -o pthread_params pthread_params.c
gcc -pthread -o pthread pthread.c

#gcc -pthread -o pthread_args pthread_args.c
#gcc -pthread -o pthread_attr pthread_attr.c
#gcc -Wall -o get_dents get_dents.c
#gcc -Wall -o move_files move_files.c
