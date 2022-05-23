CC = gcc
CFLAGS = -Wall -g

all: run clean clean

compile:
	$(CC) $(FLAGS) zad1.c -pthread -o main

run: compile
	./main 8 numbers ./images/pepper.ascii.pgm ./images/pepper_numbers.ascii.pgm
	./main 8 block ./images/pepper.ascii.pgm ./images/pepper_block.ascii.pgm

clean:
	rm -f main