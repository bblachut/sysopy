CC = gcc
CFLAGS = -Wall -g

all: run clean

compile:
	$(CC) $(FLAGS) cook.c -o cook
	$(CC) $(FLAGS) deliverer.c -o deliverer
	$(CC) $(FLAGS) main.c -o main

run: compile
	./main 5 3

clean:
	rm -f cook
	rm -f deliverer
	rm -f main