CC=gcc
CFLAGS=-Wall

all:	main

main:	main.c protocol.c receiver.c sender.c
	$(CC) $(CFLAGS) $^ -o $@
