CC=gcc

CFLAGS=-O2 -lpthread -lm
#CFLAGS=-O0 -lpthread -g # para debug

crypfs.o: crypfs.h crypfs.c 
	$(CC) $(CFLAGS) crypfs.c -c

#modificar o Makefile a partir desta linha

test1: test1.c crypfs.o  
	$(CC) test1.c crypfs.o -o test1 $(CFLAGS)
	./test1

test2: test2.c crypfs.o
	$(CC) test2.c crypfs.o -o test2 $(CFLAGS)
	./test2

test3: test3.c crypfs.o
	$(CC) test3.c crypfs.o -o test3 $(CFLAGS)
	./test3

test4: test4.c crypfs.o
	$(CC) test4.c crypfs.o -o test4 $(CFLAGS)
	./test4

test5: test5.c crypfs.o
	$(CC) test5.c crypfs.o -o test5 $(CFLAGS)
	./test5

test6: test6.c crypfs.o
	$(CC) test6.c crypfs.o -o test6 $(CFLAGS)
	./test6

test7: test7.c crypfs.o
	$(CC) test7.c crypfs.o -o test7 $(CFLAGS)
	./test7

tests2: tests2.c crypfs.o
	$(CC) tests2.c crypfs.o -o tests2 $(CFLAGS)
	./tests2

all: test1 test2 test3 test4 test5 test6 test7 tests2

#Linhas que removem os executaveis

clean1:
	rm *.o test1 arquivo1

clean2:
	rm *.o test2 arquivo2

clean3:
	rm *.o test3 arquivo3

clean4:
	rm *.o test4 arquivo4

clean5:
	rm *.o test5 arquivo5

clean6:
	rm *.o test6 arquivo6

clean7:
	rm *.o test7 arquivo7

cleanPilla: 
	rm *.o tests2 test1Pilla test2Pilla test3Pilla test4Pilla

clean all:
	rm *.o test1 test2 test3 test4 test5 test6 test7 arquivo1 arquivo2 arquivo3 arquivo4 arquivo5 arquivo6 arquivo7 tests2 test1Pilla test2Pilla test3Pilla test4Pilla
