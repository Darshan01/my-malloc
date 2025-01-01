options = -Wall -g -std=c99 -O2 -fsanitize=address,undefined
realOptions = -w -g -std=c99 -O2 -fsanitize=address,undefined

all: grind test errorTest realGrind realTest realError clean

mymalloc.o: mymalloc.c mymalloc.h
	gcc -c $(options) mymalloc.c

memgrind.o: memgrind.c mymalloc.h
	gcc -c $(options) memgrind.c
memtest.o: memtest.c mymalloc.h
	gcc -c $(options) memtest.c
errortest.o: errortest.c mymalloc.h
	gcc -c $(options) errortest.c

realGrind.o: memgrind.c mymalloc.h
	gcc -c $(realOptions) -DREALMALLOC memgrind.c -o realGrind.o
realTest.o: memtest.c mymalloc.h
	gcc -c $(options) -DREALMALLOC memtest.c -o realTest.o
realError.o: errortest.c mymalloc.h
	gcc -c $(realOptions) -DREALMALLOC errortest.c -o realError.o

grind: memgrind.o mymalloc.o
	gcc $(options) mymalloc.o memgrind.o -o memgrind
test: memtest.o mymalloc.o
	gcc $(options) mymalloc.o memtest.o -o test
errorTest: errortest.o mymalloc.o
	gcc $(options) mymalloc.o errortest.o -o errorTest

realGrind: realGrind.o mymalloc.o
	gcc $(realOptions) mymalloc.o realGrind.o -o realGrind
realTest: realTest.o mymalloc.o
	gcc $(options) mymalloc.o realTest.o -o realTest
realError: realError.o mymalloc.o
	gcc $(realOptions) mymalloc.o realError.o -o realError


demymalloc.o: mymalloc.c mymalloc.h
	gcc -c $(options) -DDEBUG mymalloc.c -o demymalloc.o
debug: demymalloc.o memgrind.o
	gcc $(options) memgrind.o demymalloc.o -o debug

clean:
	rm -f *.o
clear:
	clear
