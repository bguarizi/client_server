all: client server

client: client.o
	gcc -o client client.o biblioteca.o -pthread

client.o: client.c biblioteca.c
	gcc -c -g client.c biblioteca.c

server: server.o
	gcc -o server server.o biblioteca.o -pthread

server.o: server.c biblioteca.c
	gcc -c -g server.c biblioteca.c

clean: 
	rm *.o
