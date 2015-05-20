# Makefile for zex


hello: bin zex.o serv.o hello.o
	gcc -o bin/zex bin/zex.o bin/serv.o bin/hello.o

bin: 
	mkdir bin

zex.o: src/zex.c
	gcc -c src/zex.c -o bin/zex.o



serv.o: src/serv.c
	gcc -c src/serv.c -o bin/serv.o

hello.o: src/hello.c
	gcc -c src/hello.c	-o bin/hello.o



clear: 
	rm -f bin/*.o bin/zex
