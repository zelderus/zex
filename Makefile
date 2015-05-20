# Makefile for Hello


hello: bin zex.o serv.o hello.o
	gcc -o bin/zex bin/zex.o bin/serv.o bin/hello.o

bin: 
	mkdir bin

zex.o: zex.c
	gcc -c zex.c -o bin/zex.o

serv.o: serv.c
	gcc -c serv.c -o bin/serv.o

hello.o: hello.c
	gcc -c hello.c	-o bin/hello.o

clear: 
	rm -f bin/*.o bin/hello
