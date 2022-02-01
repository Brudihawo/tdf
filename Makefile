CFLAGS:= -Wall -Werror --pedantic --std=c11

.PHONY: tdf debug sslice test_sslice

.DEFAULT_GOAL = tdf

all: tdf sslice_test

sslice:
	$(CC) -c sslice.c $(CFLAGS)

tdf: sslice
	$(CC) -o tdf main.c sslice.o $(CFLAGS)

test_sslice: sslice
	$(CC) -o test_sslice test_sslice.c sslice.o $(CFLAGS)

debug:
	$(CC) -o tdf main.c $(CFLAGS) -g
