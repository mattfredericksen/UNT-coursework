.PHONY: all
all: pyserver client

.PHONY: silent
silent: server.c socket_io.c client
	gcc -o pyserver server.c socket_io.c

pyserver: server.c socket_io.c
	gcc -DDEBUG -o $@ $^

client: client.c socket_io.c
	gcc -o $@ $^

.PHONY: clean
clean:
	rm -f pyserver client
