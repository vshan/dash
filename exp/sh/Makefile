CC=gcc
CFLAGS=-I.

all: server.c client.c builtin.c builtin.h
	$(CC) $(CFLAGS) server.c builtin.c -o server
	$(CC) client.c -o client

clean:
	rm server client
