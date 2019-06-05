CC = gcc
CFLAGS = -g -Wall -I /usr/include -lreadline

shell:
	$(CC) -o shell shell.c $(CFLAGS)