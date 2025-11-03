CC = gcc
CFLAGS = -Wall -g

all: main

main: main.c trie.c trie.h
	$(CC) $(CFLAGS) main.c trie.c -o bio

clean:
	rm -f bio
