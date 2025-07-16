CFLAGS=-std=c11 -g -static

tdcc: tdcc.c

test: tdcc
	./test.sh

clean:
	rm -f tdcc *.o *~ tmp*

.PHONY: test clean
