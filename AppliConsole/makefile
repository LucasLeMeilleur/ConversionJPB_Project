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
