CC=gcc
CFLAGS=-Wall -g

all:	main

main:	aes.c bigdigits.c main.c protocol.c receiver.c sender.c sha3.c
	$(CC) $(CFLAGS) $^ -o $@
