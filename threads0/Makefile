all: threads

threads: threads.o
	gcc -o threads threads.o -pthread

threads.o: threads.c
	gcc -c -g threads.c

clean: 
	rm *.o