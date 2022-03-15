CC         := gcc -std=gnu11 -Wall

all:clean static

static:
	$(CC) -c wcLib.c
	ar rcs libwcLib.a wcLib.o
	$(CC) -c wcTest.c
	$(CC) wcTest.c -o wcTest libwcLib.a


shared:
	$(CC) -c -fPIC wcLib.c
	$(CC) -shared -fPIC -o libwcLib.so wcLib.o
	$(CC) wcTest.c -o wcTest -L. -lwcLib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main
	rm -f *.txt

