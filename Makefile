CFLAGS=-std=c11 -g -Wall -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

tdcc: $(OBJS)
	$(CC) -o tdcc $(OBJS) $(LDFLAGS)

$(OBJS): tdcc.h

test: tdcc
	./test.sh

clean:
	rm -f tdcc *.o *~ tmp*

.PHONY: test clean
