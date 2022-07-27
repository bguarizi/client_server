#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include "biblioteca.h"


void sendInt( int number, int socket ){
	int sendBytes = 0;

	// while (sendBytes != sizeof(int) ) {
	// 	sendBytes += send(socket, &number + sendBytes, sizeof( int ) - sendBytes, 0);
	// }

	while((sendBytes += send(socket, &number + sendBytes, sizeof( int ) - sendBytes, 0)) != sizeof(int));

	return;
	
	
}

int recvInt( int socket ){
	int num;
	int recvBytes = 0;

	// while(recvBytes != sizeof(int)){
	// 	recvBytes += recv(socket, &num + recvBytes, sizeof( int ) - recvBytes, 0);
	// }

	while((recvBytes += recv(socket, &num + recvBytes, sizeof( int ) - recvBytes, 0)) != sizeof(int));
	
	return num;
}

void sendDouble( double number, int socket ){
	send(socket, &number, sizeof( number ), 0);
}

double recvDouble( int socket ){
	double num;
	recv(socket, &num, sizeof( num ), 0);
	return num;
}

void sendString( char* string, int socket ){
	int numBytes = sizeof(char) * strlen(string);
	sendInt(numBytes, socket);
	send(socket, string, numBytes, 0);
}

char* recvString( int socket ){
	int numBytes;
	
	numBytes = recvInt(socket);
	
	char* string = (char *)calloc(numBytes+1, sizeof(char));
	
	recv(socket, string, numBytes, 0);
	
	return string;
}

void sendVoid ( void * buffer, int socket, int numBytes){
	sendInt(numBytes, socket);
	int sendBytes = 0;
	while((sendBytes += send(socket, buffer + sendBytes, numBytes - sendBytes, 0)) != numBytes);


}

void * recvVoid (int socket){
	int numBytes = recvInt(socket);
	void * buffer = (void *) calloc (numBytes, sizeof(void));
	int recvBytes = 0;
	while((recvBytes += recv(socket, buffer + recvBytes, numBytes - recvBytes, 0)) != numBytes);

	return buffer;
	

}

double randomDouble(double low, double high) {
	
	return ((double) rand()*(high - low))/(double)RAND_MAX + low;
}

int countFiles(char *directory){
	
   struct dirent* entry;
   
   int count = 0;

   //printf("Open directory in countFiles %s\n", directory);
   DIR* dir = opendir(directory);

   if (dir == NULL)
   {
      return -1;
   }

   for (;;)
   {
       entry = readdir(dir);
       if (entry == NULL)
       {
	  closedir(dir);
	  break;
       }


       
	if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
	 count++;
	}
	  
   }

   return count;

}







	
