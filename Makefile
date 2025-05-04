CC=gcc

libpenne.so: penne.o 
	$(CC) -shared -o libpenne.so penne.o -Wall -Wextra
penne.o: penne.c penne.h
	$(CC) -c -fpic penne.c -o penne.o -Wall -Wextra
clean:
	rm penne.o
