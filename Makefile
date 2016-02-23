CC=gcc
CFLAGS=-I.

all: net/protocol.c net/protocol.h net/server.c net/server.h sh/builtin.c sh/builtin.h sh/shell.c sh/shell.h util/util.c util/util.h main.c
	$(CC) $(CFLAGS) net/protocol.c net/server.c sh/shell.c util/util.c main.c -o ../build/dash

clean:
	rm dash
