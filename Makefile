CC=gcc
CFLAGS=-Wall -g

lrt:=$(shell echo "int main() {}"|gcc -x c - -lrt 2>&1)
ifeq ($(lrt),)
  CLIBS:=-lrt
else
  CLIBS:=
endif

all:	main

main:	aes.c bigdigits.c channel.c crt.c crypto.c main.c pkcs.c protocol.c receiver.c sender.c sha2.c sha3.c
		$(CC) $(CFLAGS) $^ $(CLIBS) -o $@
