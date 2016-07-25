CC=gcc

CFLAGS=-O2 -lpthread -lm
#CFLAGS=-O0 -lpthread -g # para debug

hash_s.o: hash.h hash_s.c
	$(CC) $(CFLAGS) hash_s.c -c

hash.o: hash.h hash.c
	$(CC) $(CFLAGS) hash.c -c 

#modificar o Makefile a partir desta linha

mainParalelo: main.c hash.o
	$(CC) main.c hash.o -o mainParalelo $(CFLAGS)

mainSequencial:	mainSequencial.c hash_s.o
	$(CC) mainSequencial.c hash_s.o -o mainSequencial $(CFLAGS)

all: mainParalelo

clean:
	rm *.o main
