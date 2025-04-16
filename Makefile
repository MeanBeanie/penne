CC=gcc

libpenne.so: penne.o 
	$(CC) -shared -o libpenne.so penne.o
penne.o:
	$(CC) -c -fpic penne.c -o penne.o
clean:
	rm penne.o
