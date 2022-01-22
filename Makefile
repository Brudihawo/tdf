CFLAGS:= -Wall -Werror --pedantic --std=c11 -g

.PHONY: tdf

tdf:
	$(CC) -o tdf main.c $(CFLAGS)
