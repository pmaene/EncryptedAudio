CC=gcc
CFLAGS=-Wall

all: sender receiver

sender:		sender.c
		$(CC) $(CFLAGS) $^ -o $@

receiver:	receiver.c
		$(CC) $(CFLAGS) $^ -o $@

