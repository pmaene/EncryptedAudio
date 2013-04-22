CC=gcc
CFLAGS=-O2 -Wall -g
CLIBS=

UNAME=$(shell uname)
ifeq ($(UNAME), Darwin)
    CFLAGS=-Wall -g
endif

LRT=$(shell echo "int main() {}"|gcc -x c - -lrt 2>&1)
ifeq ($(LRT), )
    CLIBS=-lrt
endif

all:	main

main:	aes.c bigdigits.c buffer.c channel.c crt.c crypto.c main.c pkcs.c protocol.c random.c receiver.c sender.c sha1.c sha2.c sha3.c
		$(CC) $(CFLAGS) $^ $(CLIBS) -o $@
