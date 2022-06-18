all: main

lib: lib/libfilesearch.all

main: dist/main.bin

dist/lib.o: src/lib.cpp
	g++ -O -c src/lib.cpp -o dist/lib.o

lib/libfilesearch.a: dist/lib.o
	ar rcs lib/libfilesearch.a dist/lib.o

dist/main.bin: src/main.cpp src/lib.hpp lib/libfilesearch.a
	g++ src/main.cpp -L./lib -lfilesearch -o dist/main.bin