CC         := gcc -std=gnu11 -Wall

default: compile run clean

compile:
	$(CC) zad2.c -o my_program -lm

run:
	./my_program 0.0000001 1
	./my_program 0.0000001 5
	./my_program 0.0000001 10
	./my_program 0.00000001 1
	./my_program 0.0000001 5
	./my_program 0.0000001 10


clean:
	rm -f my_program