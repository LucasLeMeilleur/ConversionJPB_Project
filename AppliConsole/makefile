CC = g++
CFLAGS = -Wall
LDFLAGS = -ljpeg 

main: main.o conversion.o
	g++ -o programme.exe main.cpp conversion.cpp -ljpeg -lpng

main.o: main.cpp
	$(CC) $(CFLAGS) -c $<

conversion.o: conversion.cpp
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f *.o main

window: 
	x86_64-w64-mingw32-g++ -o programme.exe main.cpp conversion.cpp -ljpeg -lpng -lz -static
	
linux: 
	g++ -o programme main.cpp conversion.cpp -L/opt/libjpeg-turbo/lib64 -I/opt/libjpeg-turbo/include -ljpeg -lpng -lz -static

