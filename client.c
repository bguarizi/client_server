#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include "biblioteca.h"

#define MAXRCVLEN 5000
#define MAX_COMMAND_LENGHT 5000 

void sendFilesName(int mysocket, char* client){

        int num = countFiles(client);
        //printf("NUMERO DE ARQUIVOS: %d\n", num);
        sendInt(num, mysocket);

        sendString(client, mysocket); //ENVIA NOME CLIENTE

        struct dirent* entry;

        char * diretorio = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));
        strcat(strcpy(diretorio, "./"), client);

        DIR* dir = opendir(diretorio);

        if (dir == NULL)
        {
        //printf("Can't find directory %s\n", directory);
        return;
        }

        for (;;)
        {
                entry = readdir(dir);

                if (entry == NULL)
                {
                        //printf("fim do diretorio %s\n", directory);
                        closedir(dir);
                        break;
                }

                
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ // Inserir os arquivos do cliente na estrutura de dados dele
                        
                        sendString(entry->d_name, mysocket);
                        

                }
        
        }

}

void list(int mysocket){

        sendInt(COMMAND_LIST, mysocket);

        // void * v = recvVoid(mysocket);
        //printf("end V: %p\n", v);
        //char * aloha = (char*) v;
        //printf("aloha is: %s\n", aloha);

        int numClients = recvInt(mysocket); // QUANTIDADE DE CLIENTES

        char * recvChar;

        for (int i = 0; i < numClients; i++){

                recvChar = recvString(mysocket); // NOME DO CLIENTE

                int numarquivos = recvInt(mysocket);

                printf("%s:\n", recvChar);
                
                // file *aux2 = aux->files;
                
                for (int i = 0; i < numarquivos; i++){
                        // printf("1\n");
                        recvChar = recvString(mysocket);
                        // printf("2\n");
                        printf(" %s\n", recvChar);
                        // aux2 = aux2 -> next;
                }

        }
         
        printf("\n\n--- Fim da Lista ---\n\n");

}

void sendFile(char * diretorio, int socket){

        FILE * file;

        void * conteudo = (void*) calloc (MAX_COMMAND_LENGHT,sizeof (void));

        printf("SEND FILE\n");
        printf("diretório: %s\n", diretorio);

        file = fopen(diretorio, "r");

        if (file != NULL) {
                printf("ABRIU\n");
                
                struct stat sb;
                int numBytesRead = 0;

                stat(diretorio, &sb);
                printf("TAMANHO ARQUIVO: %ld\n", sb.st_size);

                // while (numBytesRead < sb.st_size)
                // {
                        printf("LI AQUI\n");
                        numBytesRead = fread(conteudo, sizeof(void), MAX_COMMAND_LENGHT,file);
                        
                        printf("conteudo lido: %s\n", (char*)conteudo);
                        printf("numero: %d\n", numBytesRead);
                
                // }
                

                
                


        } else {
                printf("\n\nAVISO: ARQUIVO NÃO ENCONTRADO!\n\n");
        }


}

void connectClient(int port, char * ip, char * file, int command){

        // printf("PORTA %d\n", port);
        // printf("IP %s\n", ip);

        char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
        bzero( buffer, MAXRCVLEN + 1 );
        int len, socket_conClient;
        struct sockaddr_in dest;

        socket_conClient = socket(AF_INET, SOCK_STREAM, 0);

        memset(&dest, 0, sizeof(dest));                /* zero the struct */
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
        dest.sin_port = htons( port );                /* set destination port number */

        int connectResult = connect(socket_conClient, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

        if( connectResult == - 1 ){

                printf("CLIENT ERROR: %s\n", strerror(errno));

                return;
        }

        if (command == COMMAND_DELETE) {
                sendInt(command, socket_conClient); // ENVIA O COMANDO DESEJADO PARA O CLIENTE
                sendString(file, socket_conClient); // ENVIA O NOME DO ARQUIVO 
        } else if (command == COMMAND_SEND){
                sendInt(command, socket_conClient); // ENVIA O COMANDO DESEJADO PARA O CLIENTE
                // sendString(file, socket_conClient); // ENVIA O NOME DO DIRETÓRIO
                sendFile(file, socket_conClient);
        }

        

        printf("VOU CONECTAR COM ELE!\n");
        
}

void * delet(int consocket, char * fileDelete){

        if (recvInt(consocket)){ // ARQUIVO ENCONTRADO

                int port = recvInt(consocket);
                char * ip = recvString(consocket);
                connectClient(port, ip, fileDelete, COMMAND_DELETE);
        } else {
                printf("\n\nAVISO: Arquivo Inexistente!\n\n");
        }
}

void connectSendFile(int consocket, char * client, char * filesSend){

        printf("CONNECT SEND FILE\n");

        if (recvInt(consocket)){ // CLIENTE ENCONTRADO

                printf("FIND\n");

                char * diretorio = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));

                int port = recvInt(consocket);
                char * ip = recvString(consocket);

                char * c = strtok(filesSend, " ");

                char * file = strtok(NULL, " ");

                printf("MEU FILE: %s\n", file);

                strcat(strcpy(diretorio, "./"), client);
                strcat(strcat(diretorio, "/"), file);

                printf("MEU DIRETÓRIO: %s\n", diretorio);

                connectClient(port, ip, diretorio, COMMAND_SEND);

                // while (file != NULL){
                //         file = strtok(NULL, " ");
                //         connectClient(port, ip, file, COMMAND_SEND);
                // }

        } else {
                printf("\n\nAVISO: Cliente Inexistente!\n\n");
        }

}

void * sendCommands(void * arguments){ // ENVIA COMANDOS AO SERVIDOR GERAL

        struct parametros * par = (struct parametros*) arguments;
        int mysocket = par->consocket;
        char * client = par->client;

        char * filesDelete = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));
        char * fileSendDelete = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));
        char * filesSend = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));

        while (1) {

                int command;

                printf("Digite o comando desejado: \n");
                printf(" Para listar os clientes e arquivos digite 0.\n");
                printf(" Para visualizar o status digite 1.\n");
                printf(" Para realizar o envio dos arquivos digite 2.\n");
                printf(" Para sair digite 3.\n");
                printf(" Para deletar um arquivo digite 4.\n");
                printf(" Para enviar um arquivo digite 5.\n");


                scanf("%d", &command);

                switch(command){
                        case COMMAND_LIST:
                                system("clear");
                                list(mysocket);
                                break;
                        case COMMAND_STATS:
                                system("clear");
                                break;
                        case SENDFILENAMELIST:
                                sendInt(SENDFILENAMELIST, mysocket); // ENVIO DO COMMANDO DE SEND FILES
                                sendFilesName(mysocket, client);
                                break;
                        case COMMAND_EXIT:
                                system("clear");
                                printf("BYE BYE!\n");
                                sendInt(COMMAND_EXIT, mysocket); // ENVIO DO COMMANDO DE EXIT
                                sendString(client, mysocket);
                                close(mysocket);
                                return EXIT_SUCCESS;
                                break;
                        case COMMAND_DELETE:
                                system("clear");
                                
                                printf("Digite os arquivos que deseja deletar: \n");
                
                                scanf("%s", filesDelete);
                                printf("filesDelete: %s\n\n", filesDelete);

                                char * file1 = strtok(filesDelete, " ");

                                while(file1 != NULL){
                                        sendInt(COMMAND_DELETE, mysocket); // ENVIO DO COMMANDO DE SEND FILES
                                        printf("file: %s\n", file1);
                                        sendString(file1, mysocket); // ENVIO DO ARQUIVO A SER DELETADO
                                        delet(mysocket, file1);
                                        file1 = strtok(NULL, " ");
                                }                                

                                break;
                        case COMMAND_SEND:
                                system("clear");

                                printf("Digite o cliente e os arquivos que deseja enviar: \n");
                
                                scanf(" %[^\n]", filesSend);

                                char * copy = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));
                                strcpy(copy, filesSend);

                                // fgets(filesSend, 256, stdin);
                                
                                printf("filesSend: %s\n\n", filesSend);

                                char * c = strtok(filesSend, " ");

                                /* MANDA PARA O SERVIDOR QUAL O CLIENTE QUE DESEJO ME CONECTAR */

                                sendInt(COMMAND_SEND, mysocket); // ENVIO DO COMMANDO DE SEND
                                
                                sendString(c, mysocket); // ENVIO DO NOME DO CLIENTE
                                
                                connectSendFile(mysocket, client, copy); 
                                // printf("2; %d\n", recvInt(mysocket));                         
                                break;
                }
        }
}

void * recvCommands(void * arguments) { // RECEBE COMANDOS DE UM CLIENTE
        
        struct parametros * par = (struct parametros*) arguments;

        char * client = par->client;
        int socket_otherclient = par->consocket;
        int socket_serv = par->socket_serv;

        int command = recvInt(socket_otherclient);

        printf("COMMAND: %d\n", command);

        char * fileDelete = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));

        switch(command){

                case COMMAND_DELETE:
                        fileDelete = recvString(socket_otherclient);

                        printf("REMOÇÃO DO ARQUIVO %s\n", fileDelete);
                        
                        char * diretorio = (char*) calloc (MAX_COMMAND_LENGHT,sizeof (char));
                        strcat(strcpy(diretorio, "./"), client);

                        strcat(strcat(diretorio, "/"), fileDelete);
                        
                        printf("DIRENT IS %s\n", diretorio);
                        remove(diretorio); // REMOVER O ARQUIVO QUE O CLIENTE CONECTADO ME ENVIAR
                        printf("REMOÇÃO DO ARQUIVO %s\n", fileDelete);
                        
                        // sendString(fileDelete, socket_serv); // ENVIA O ARQUIVO QUE FOI DELETADO PARA O SERVIDOR GERAL
                        
                        break;

        }


}

void * initServer(void * arguments){

        struct parametros * par1 = (struct parametros*) arguments;

        int port = par1->port;
        char * client = par1->client;

        // int port = *(int*) argument;

        printf(" ---------- PORTA RECEBIDA %d -------------\n", port);

        srand( time( NULL ));

        struct sockaddr_in dest; /* socket info about the machine connecting to us */
        struct sockaddr_in serv; /* socket info about our server */
        int mysocket;            /* socket used to listen for incoming connections */
        socklen_t socksize = sizeof(struct sockaddr_in);

        memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
        serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
        serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
        serv.sin_port = htons( port );           /* set the server port number */

        mysocket = socket(AF_INET, SOCK_STREAM, 0);

        /* bind serv information to mysocket */
        bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

        /* start listening, allowing a queue of up to 1 pending connection */
        listen(mysocket, 1);


        printf("Server is waiting for connections on port:%d\n", port );

        int consocket_serv_client;

        while (1){
                
                // ESPERANDO POR UMA CONEXÃO
                consocket_serv_client = accept(mysocket, (struct sockaddr *)&dest, &socksize);
                
                printf("SE CONECTOU COMIGOOOOO!!!!\n");

                // int * con = &consocket_serv_client;
        
                pthread_t thread; // inicializar uma nova thread

                parametros * par = (parametros*) calloc (1,sizeof (parametros));

                par->consocket = consocket_serv_client;
                par->client = client;
                par->socket_serv = par1->consocket;

                // printf("consocket is %d\n", consocket);
                // printf("par->consocket is %d\n", par->consocket);

                pthread_create( &thread, NULL, recvCommands, par);
                
        }
        

        // close(consocket_serv_client);

}

int main(int argc, char *argv[])
{

        if( argc != 4 ){ 

                printf("USAGE: server port_number\n");

                return EXIT_FAILURE;

        }

        char *client = argv[1];

        srand( time( NULL ));

        char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
        bzero( buffer, MAXRCVLEN + 1 );
        int len, mysocket;
        struct sockaddr_in dest;

        mysocket = socket(AF_INET, SOCK_STREAM, 0);

        memset(&dest, 0, sizeof(dest));                /* zero the struct */
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
        dest.sin_port = htons(  atoi( argv[ 3 ]) );                /* set destination port number */

        int connectResult = connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

        if( connectResult == - 1 ){

                printf("CLIENT ERROR: %s\n", strerror(errno));

                return EXIT_FAILURE;
        }

        pthread_t thread_serv, thread_client; // inicializar uma nova thread

        int port_serv_client = atoi(argv[2]);

        sendInt(SENDFILENAMELIST, mysocket); // ENVIO DO COMMANDO DE SEND FILES

        sendInt(port_serv_client, mysocket); // ENVIANDO A PORTA DO CLIENTE

        // int * arg = &port_serv_client;
        int * socket = &mysocket;

        sendFilesName(mysocket, client);

        parametros * par = (parametros*) calloc (1,sizeof (parametros));

        par->consocket = mysocket;
        par->client = client;
        par->port = port_serv_client;

        pthread_create( &thread_serv, NULL, initServer, par); // CRIA A THREAD QUE INICIALIZA O SERVIDOR

        pthread_create( &thread_client, NULL, sendCommands, par); // CRIA A THREAD QUE ESPERA POR ALGUMA SOLICITAÇÃO DO CLIENTE

        pthread_join( thread_serv, NULL );
        pthread_join( thread_client, NULL );
        

        close(mysocket);
        return EXIT_SUCCESS;
}

//send f1 123