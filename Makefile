all:p3test
p3test:p3test.o fs.o disk.o
	gcc -o p3test test.o fs.o disk.o

p3test.o:test.c fs.h
	gcc -c test.c

fs.o:fs.c fs.h
	gcc -c fs.c

disk.o:disk.c disk.h
	gcc -c disk.c
