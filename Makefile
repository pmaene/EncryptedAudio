SOURCES=aes.c bigdigits.c buffer.c channel.c crt.c crypto.c decode.c encode.c functions.c main.c nettle.c protocol.c random.c receiver.c sender.c sha1.c sha2.c sha3.c wavpcm_io.c

CC=gcc
CFLAGS=-Wall
CLIBS=

UNAME=$(shell uname)
ifeq ($(UNAME), Darwin)
    CFLAGS=-Wall
endif

LRT=$(shell echo "int main() {}" | gcc -x c - -lrt 2>&1)
ifeq ($(LRT), )
    CLIBS=-lrt
endif

default: debug

debug: $(SOURCES)
	@echo "Building for $@"
	@$(CC) $(CFLAGS) -g3 $^ $(CLIBS) -o main

release: $(SOURCES)
	@echo "Building for $@"
	@$(CC) $(CFLAGS) -O3 $^ $(CLIBS) -o main
