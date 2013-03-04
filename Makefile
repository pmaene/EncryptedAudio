CC=gcc
CFLAGS=-Wall
CFILES=

all: encryption

encrypion: $(CFILES)
           $(CC) $(CFILES) $^ -o $@


