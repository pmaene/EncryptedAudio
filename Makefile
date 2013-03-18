CC=gcc
CFLAGS=-Wall -g

all:	main

main:	aes.c bigdigits.c crypto.c main.c montgomery.c protocol.c receiver.c sender.c sha3.c
		$(CC) $(CFLAGS) $^ -lrt -o $@
