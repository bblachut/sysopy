CC = gcc
CFLAGS = -Wall -g

all: run clean

compile:
	$(CC) $(FLAGS) cook.c -o cook -lpthread -lrt
	$(CC) $(FLAGS) deliverer.c -o deliverer -lpthread -lrt
	$(CC) $(FLAGS) main.c -o main -lpthread -lrt

run: compile
	./main 5 3

clean:
	rm -f cook
	rm -f deliverer
	rm -f main