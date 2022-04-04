CC         := gcc -std=gnu11 -Wall

default: clean compile run clean

compile:
	$(CC) zad1_exec.c -o exec
	$(CC) zad1_fork.c -o fork
	$(CC) to_exec.c -o to_exec

run:
	./exec ignore
	./exec mask
	./exec pending

	./fork ignore
	./fork handler
	./fork mask
	./fork pending

clean:
	rm -f exec
	rm -f fork
	rm -f to_exec