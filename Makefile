CC=gcc
CFLAGS=-Wall -g

lrt:=$(shell echo "int main() {}"|gcc -x c - -lrt 2>&1)
ifeq ($(lrt),)
  CLIBS:=-lrt
else
  CLIBS:=
endif

all:	main

main:	aes.c bigdigits.c channel.c crypto.c main.c montgomery.c protocol.c receiver.c sender.c sha3.c
		$(CC) $(CFLAGS) $^ $(CLIBS) -o $@
