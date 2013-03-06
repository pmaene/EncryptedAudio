CC=gcc
CFLAGS=-Wall

all:	main

main:	main.c
	$(CC) $(CFLAGS) $^ -o $@
