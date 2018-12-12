CC=gcc
CFLAGS=-I

ALL: filesystem
	
filesystem: filesystem.o
	$(CC) -o filesystem filesystem.c $(CFLAGS).

clean:
	rm filesystem.o filesystem
