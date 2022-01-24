CFLAGS:= -Wall -Werror --pedantic --std=c11

.PHONY: tdf debug

tdf:
	$(CC) -o tdf main.c $(CFLAGS)

debug:
	$(CC) -o tdf main.c $(CFLAGS) -g
