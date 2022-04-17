CC = gcc
CFLAGS = -Wall -g

all: run clean

compile:
	$(CC) $(FLAGS) zad1.c -o main

run: compile
	./main commands.txt

clean:
	rm -f main