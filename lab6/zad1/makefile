CC = gcc
CFLAGS = -Wall -g

all: compile

compile:
	$(CC) $(FLAGS) client.c -o client
	$(CC) $(FLAGS) server.c -o server

clean:
	rm -f client
	rm -f server