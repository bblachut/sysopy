CC         := gcc -c -std=gnu11 -Wall

all: static shared clean

static:
	$(CC) -c wcLib.c
	ar rcs libwcLib.a wcLib.o
	$(CC) wcTest.c -o wcTest -L. -lwcLib


shared:
	$(CC) -c -fPIC wcLib.c
	$(CC) -shared -fPIC -o libwcLib.so wcLib.o
	$(CC) wcTest.c -o wcTest -L. -lwcLib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so