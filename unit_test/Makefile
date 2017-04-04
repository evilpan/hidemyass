CC=gcc
all: test

test: test.c ../core.c
	$(CC) -o $@ $^ -g -Wall
clean:
	rm -rf test

.PHONY: clean
