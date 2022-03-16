CC         := gcc -std=gnu11 -Wall

all:clean static

static:
	$(CC) -c wcLib.c
	ar rcs libwcLib.a wcLib.o
	$(CC) -c wcTest.c
	$(CC) wcTest.c -o wcTest libwcLib.a


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f wcTest

