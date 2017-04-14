#!/bin/bash

gcc -pthread -o pthread pthread.c && time ./pthread;
