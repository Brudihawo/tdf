CFLAGS:= -Wall -Werror --pedantic --std=c11

.PHONY: tdf debug sslice test_sslice

.DEFAULT_GOAL = tdf

all: tdf test_sslice

sslice:
	$(CC) -c sslice.c $(CFLAGS)

tdf: sslice
	$(CC) -o tdf main.c sslice.o $(CFLAGS)

test_sslice: sslice
	$(CC) -o test_sslice test_sslice.c sslice.o $(CFLAGS)

debug:
	$(CC) -o tdf main.c $(CFLAGS) -g

compiledb:
	compiledb make -Bnwk all

clean:
	rm -vfr **/*.o
	rm -vfr *.o
	rm -vf ./tdf
	rm -vf test_sslice
