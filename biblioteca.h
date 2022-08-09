#define COMMAND_LIST 0
#define COMMAND_STATS 1
#define SENDFILENAMELIST 2
#define COMMAND_EXIT 3
#define COMMAND_DELETE 4
#define COMMAND_SEND 5


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



