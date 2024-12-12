CC=gcc
CFLAGS=-Wall

all: filosofos filosofos2

filosofos: main.c
	$(CC) $(CFLAGS) -o $@.out $^

filosofos2: main.c
	$(CC) $(CFLAGS) -DUSE_TIME_WAIT -o $@.out $^