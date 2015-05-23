# Makefile for zex


hello: bin zex_types.o zex.o responser.o serv.o hello.o
	gcc -o bin/zex bin/zex.o bin/zex_types.o bin/responser.o bin/serv.o bin/hello.o

bin: 
	mkdir bin

zex.o: src/zex.c
	gcc -c src/zex.c -o bin/zex.o


zex_types.o: src/zex_types.c
	gcc -c src/zex_types.c -o bin/zex_types.o

responser.o: src/responser.c
	gcc -c src/responser.c -o bin/responser.o


serv.o: src/serv.c
	gcc -c src/serv.c -o bin/serv.o

hello.o: src/hello.c
	gcc -c src/hello.c	-o bin/hello.o



clear: 
	rm -f bin/*.o bin/zex
