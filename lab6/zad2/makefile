CC = gcc
CFLAGS = -Wall -g

all: compile

compile:
	$(CC) $(FLAGS) client.c -o client -lrt
	$(CC) $(FLAGS) server.c -o server -lrt

clean:
	rm -f client
	rm -f server