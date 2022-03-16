CC         := gcc -std=gnu11 -Wall

all:clean static

static:
	make clean
	$(CC) -c wcLib.c
	ar rcs libwcLib.a wcLib.o
	$(CC) -c wcTest.c
	$(CC) wcTest.c -o wcTest libwcLib.a -O1


shared:
	make clean
	$(CC) -c -fPIC wcLib.c
	$(CC) -shared -fPIC -o libwcLib.so wcLib.o
	$(CC) -c wcTest.c
	$(CC) wcTest.c -o wcTest -L. -lwcLib -Wl,-rpath=`pwd` -O2

dynamic:
	make clean
	$(CC) -fPIC -c wcLib.c
	$(CC) -Wl,-rpath=. -fPIC -shared -o libwcLib.so wcLib.o
	$(CC) -o wcTestDyn wcTestDyn.c -ldl -D DYNAMIC -O3


clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f wcTest