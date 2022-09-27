#include <time.h>

#define COMMAND_LIST 0
#define COMMAND_STATS 1
#define SENDFILENAMELIST 6
#define COMMAND_EXIT 5
#define COMMAND_DELETE 3
#define COMMAND_SEND 4
#define COMMAND_RECV 2

// struct tm {
// 	int tm_sec; //representa os segundos de 0 a 59
// 	int tm_min; //representa os minutos de 0 a 59
// 	int tm_hour; //representa as horas de 0 a 24
// 	int tm_mday: //dia do mês de 1 a 31
// 	int tm_mon; //representa os meses do ano de 0 a 11
// 	int tm_year; //representa o ano a partir de 1900
// 	int tm_wday; //dia da semana de 0 (domingo) até 6 (sábado)
// 	int tm_yday; // dia do ano de 1 a 365
// 	int tm_isdst; //indica horário de verão se for diferente de zero
// };

typedef struct file {
	char *name;
	struct file *next;

} file;

typedef struct client {
	int id;
	char *name;
	struct client *next;
	int numFiles;
	int port;
	char * ip;
	file *files;

} client;

typedef struct clients_list{
	client *first;
	client *last;

} clients_list;

typedef struct parametros {
	int consocket;
	char * client;
	clients_list * cl;
	char * ip;
	int port;
	int socket_serv;
	char * c;

} parametros;

void sendInt( int number, int socket );
int recvInt( int socket );

void sendDouble( double number, int socket );
double recvDouble( int socket );

void sendString( char* string, int socket );
char* recvString( int socket );

void sendVoid( void * buffer, int socket, int numBytes);
void * recvVoid( int socket );

double randomDouble(double low, double high);

char mystrcat(char * first, char* second);

int countFiles(char *directory);

void listFile(int socket, clients_list *cl);

clients_list* insertClients(char *directory, int socket, clients_list * cl);



