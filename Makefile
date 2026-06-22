CFLAGS=-Wall -Wextra -std=c11 -pedantic
LIBS=

bm: main.c
	$(CC) $(CFLAGS) -o vmc main.c $(LIBS)
