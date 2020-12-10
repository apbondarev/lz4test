default: lz4test

lz4.o: lz4.h lz4.c
	gcc -c lz4.c

lz4test.0: lz4test.c
	gcc -c lz4test.c

lz4test: lz4.o lz4test.0 lz4.preprocessed.c
	gcc -o lz4test lz4test.o lz4.o

lz4.preprocessed.c: lz4.h lz4.c
	cpp -o lz4.preprocessed.c lz4.c
clean:
	rm -f *.o lz4.preprocessed.c lz4test
