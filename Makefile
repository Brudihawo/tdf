CFLAGS:= -Wall -Werror --pedantic --std=c11

.PHONY: tdf debug sslice test_sslice

.DEFAULT_GOAL = tdf

all: tdf

tdf:
	$(CC) -o tdf main.c $(CFLAGS)

debug:
	$(CC) -o tdf main.c $(CFLAGS) -g -fsanitize=address,leak,undefined

compiledb:
	compiledb make -Bnwk all

clean:
	rm -vfr **/*.o
	rm -vfr *.o
	rm -vf ./tdf
