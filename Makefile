CC=gcc
CFLAGS=-Wall

all:	main

main:	bigdigits.c main.c protocol.c receiver.c sender.c
	$(CC) $(CFLAGS) $^ -o $@
