CC=gcc
CFLAGS=-Wall
CFILES=

all: crypto

crypto:	$(CFILES)
	$(CC) $(CFLAGS) $^ -o $@
