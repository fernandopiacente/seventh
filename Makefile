CC = gcc
CFLAGS = -Wall -Wextra -pedantic

all: client server

client: client_main.c
	$(CC) $(CFLAGS) -o client client_main.c client_functions.c

server: server_main.c
	$(CC) $(CFLAGS) -o server server_main.c server_functions.c

clean:
	rm -f client server
