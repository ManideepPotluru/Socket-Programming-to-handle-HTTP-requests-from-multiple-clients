all: webserver client
webserver: webserver.c
	gcc -O3 -o webserver webserver.c
client: client.c utils.h
	gcc -O3 -o client client.c
clean:
	rm webserver client
