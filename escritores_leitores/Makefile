CC=gcc
CFLAGS=-Wall

all: bin/escritor_starvation bin/escritor-solucao bin/escritor-solucao01 bin/escritor-solucao02 bin/escritor-solucao03

bin/escritor_starvation: escritor_starvation.c
	$(CC) $(CFLAGS) -o $@.out $^

bin/escritor-solucao: escritor-solucao.c
	$(CC) $(CFLAGS) -o $@.out $^

bin/escritor-solucao01: escritor-solucao01.c
	$(CC) $(CFLAGS) -o $@.out $^

bin/escritor-solucao02: escritor-solucao02-invalida.c
	$(CC) $(CFLAGS) -o $@.out $^

bin/escritor-solucao03: escritor-solucao03.c
	$(CC) $(CFLAGS) -o $@.out $^