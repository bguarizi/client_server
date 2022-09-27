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
#define MAX_LENGHT 5000 

int globalCommand = -1;

void sendFilesName(int mysocket, char* client){

        int num = countFiles(client);

        sendInt(num, mysocket);

        sendString(client, mysocket); //ENVIA NOME CLIENTE

        struct dirent* entry;

        char * diretorio = (char*) calloc (MAX_LENGHT,sizeof (char));
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

        int numClients = recvInt(mysocket); // QUANTIDADE DE CLIENTES

        char * recvChar;

        for (int i = 0; i < numClients; i++){

                recvChar = recvString(mysocket); // NOME DO CLIENTE

                int numarquivos = recvInt(mysocket);

                printf("\n---> %s:\n\n", recvChar);
                
                // file *aux2 = aux->files;
                
                for (int i = 0; i < numarquivos; i++){
                        // printf("1\n");
                        recvChar = recvString(mysocket);
                        // printf("2\n");
                        printf("> '%s'\n", recvChar);
                        // aux2 = aux2 -> next;
                }

        }
         
        printf("\n\n--- Fim da Lista ---\n\n");

}

void status(int socket){

        // printf("socket %d\n", socket);

        int quantClients = recvInt(socket);

        printf("O servidor possui %d clientes conectados\n\n", quantClients);

        for (int i = 0; i < quantClients; i++){
                char * c = recvString(socket);
                int i = recvInt(socket);

                printf("> %s - Possui %d arquivos.\n", c, i);
        }

        int ano = recvInt(socket);
        int mes = recvInt(socket);
        int dia = recvInt(socket);

        printf("\nData de hoje ........: %d/%d/%d \n", ano, mes, dia);  
  
        // //para retornar o mês corretamente devemos adicionar +1 
        // //como vemos abaixo
        // printf("\nMes..........: %d\n", );
        
        // //para retornar o ano corretamente devemos adicionar 1900 
        // //como vemos abaixo
        // printf("\nAno..........: %d\n\n", recvInt(socket));  

        printf("Hora ........: %d:",recvInt(socket));//hora   
        printf("%d:",recvInt(socket));//minuto
        printf("%d\n",recvInt(socket));//segundo 

        puts("");

}

void sendFile(char * diretorio, char * client_send, int socket){

        FILE * file;

        file = fopen(diretorio, "r");

        void * conteudo = (void*) calloc (MAX_LENGHT,sizeof (void));

        int count = 0;

        char * name_file = (char*) calloc (MAX_LENGHT,sizeof (char));

        for (int i = 0; i < strlen(diretorio); i++){
                if (diretorio[i] == '/' && count != 0){
                        for (int j = i+1; j < strlen(diretorio); j++){
                                name_file[count-1] = diretorio[j];
                                count++;
                        }
                        break;
                } else if (diretorio[i] == '/') {
                        count = 1;
                }
        }

        char * newFile = (char*) calloc (MAX_LENGHT,sizeof (char));

        strcat(strcpy(newFile, "./"), client_send);
        strcat(strcat(newFile, "/"), name_file);

        sendString(newFile, socket); // ENVIA O NOME DO ARQUIVO
        
        struct stat sb;
        int numBytesRead = 0;

        int numBytesTotal = 0;

        stat(diretorio, &sb);

        // printf("TAMANHO ARQUIVO: %ld\n", sb.st_size);

        sendInt(sb.st_size, socket);

        while (numBytesTotal < sb.st_size)
        {

                numBytesRead = fread(conteudo, sizeof(void), MAX_LENGHT,file);
              
                numBytesTotal += numBytesRead;

                sendInt(numBytesRead, socket); //ENVIA O NÚMERO DE BYTES LIDO

                sendVoid(conteudo, socket, numBytesRead);

        
        }
}

void * recvFileSend(void * arguments){

        struct parametros * par = (struct parametros*) arguments;

        int socket = par->consocket;
        char * name_file = par->client;

        name_file = recvString(socket);

        FILE * f;

        printf("DIRENT %s\n", name_file);
        
        f = fopen(name_file, "w+");

        int numTotalBytes = recvInt(socket);

        int numRecvBytes = 0;

        while (numRecvBytes < numTotalBytes)
        {
                int numBytes = recvInt(socket);
                printf("BYTES RECEBIDOS: %d\n", numBytes);
                
                void * conteudo = recvVoid(socket);

                printf("CONTEUDO RECEBIDO: %s\n", (char*)conteudo);

                int bytes = fwrite(conteudo, sizeof(void), numBytes, f);

                fflush(f);

                numRecvBytes += bytes;

                printf("BYTES ESCRITOS: %d\n\n", bytes);
 
        }    

}

void recvFile(int socket){

        char * name_file = (char*) calloc (MAX_LENGHT,sizeof (char));
        char * dirent = (char*) calloc (MAX_LENGHT,sizeof (char));
        name_file = recvString(socket);
        dirent = recvString(socket);

        int find = recvInt(socket);

        if (find) {

                FILE * f;

                f = fopen(name_file, "w+");

                int numTotalBytes = recvInt(socket);

                int numRecvBytes = 0;

                while (numRecvBytes < numTotalBytes)
                {
                        int numBytes = recvInt(socket);
        
                        void * conteudo = recvVoid(socket);

                        int bytes = fwrite(conteudo, sizeof(void), numBytes, f);

                        fflush(f);

                        numRecvBytes += bytes;

                }    

        } else {
                printf("\n\nAVISO: ARQUIVO NO PATH %s NÃO ENCONTRADO!\n", dirent);
        }

        printf("FIM DO RECV FILE EM %s\n",dirent);

        

}

void * sendFileRecv(void * arguments){

        struct parametros * par = (struct parametros*) arguments;

        int socket = par->consocket;
        char * diretorio = par->client;
        char * client_send = par->c;
    
        FILE * file;

        file = fopen(diretorio, "r");

        void * conteudo = (void*) calloc (MAX_LENGHT,sizeof (void));

        int count = 0;

        char * name_file = (char*) calloc (MAX_LENGHT,sizeof (char));

        for (int i = 0; i < strlen(diretorio); i++){
                if (diretorio[i] == '/' && count != 0){
                        for (int j = i+1; j < strlen(diretorio); j++){
                                name_file[count-1] = diretorio[j];
                                count++;
                        }
                        break;
                } else if (diretorio[i] == '/') {
                        count = 1;
                }
        }

        char * newFile = (char*) calloc (MAX_LENGHT,sizeof (char));

          
        strcat(strcpy(newFile, "./"), client_send);
        strcat(strcat(newFile, "/"), name_file);

        sendString(newFile, socket); // ENVIA O NOME DO ARQUIVO
        sendString(diretorio, socket); // ENVIA O NOME DO ARQUIVO
        

        if (file != NULL) {

                sendInt(1, socket);

                struct stat sb;
                int numBytesRead = 0;

                int numBytesTotal = 0;

                stat(diretorio, &sb);
                // printf("TAMANHO ARQUIVO: %ld\n", sb.st_size);

                sendInt(sb.st_size, socket);

                while (numBytesTotal < sb.st_size)
                {
                        numBytesRead = fread(conteudo, sizeof(void), MAX_LENGHT,file);
                        // printf("FILE LIDO: %s\n", newFile);
                        numBytesTotal += numBytesRead;
                        
                        sendInt(numBytesRead, socket); //ENVIA O NÚMERO DE BYTES LIDO

                        sendVoid(conteudo, socket, numBytesRead);

                
                }

        } else {
                sendInt(0, socket);

        }

        
}

void * connectClient(void * arguments){

        // printf("PORTA %d\n", port);
        // printf("IP %s\n", ip);

        struct parametros * par = (struct parametros*) arguments;

        int port = par->port;
        char * ip = par->ip;
        char * file = par->client;
        int command = par->consocket;
        char * client_sr = par->c;

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

                return NULL;
        }

        if (command == COMMAND_DELETE) {
                sendInt(command, socket_conClient); // ENVIA O COMANDO DESEJADO PARA O CLIENTE
                sendString(file, socket_conClient); // ENVIA O NOME DO ARQUIVO 
        } else if (command == COMMAND_SEND){
                
                sendInt(command, socket_conClient); // ENVIA O COMANDO DESEJADO PARA O CLIENTE
                sendFile(file, client_sr, socket_conClient);
        
                        
        } else if (command == COMMAND_RECV){

                sendInt(command, socket_conClient); // ENVIA O COMANDO DESEJADO PARA O CLIENTE

                sendString(client_sr, socket_conClient); //envia cliente que quer receber arquivos
                sendString(file, socket_conClient); //envia path que quer receber arquivos

                // printf(":::::%s\n", file);

                recvFile(socket_conClient);
     
        }

        close(socket_conClient);

        
}

void * delet(int consocket, char * fileUse){

        if (recvInt(consocket)){ // ARQUIVO ENCONTRADO

                int port = recvInt(consocket);
                char * ip = recvString(consocket);
                
                parametros * par = (parametros*) calloc (1,sizeof (parametros));

                par->port = port;
                par->ip = ip;
                par->client = fileUse;
                par->consocket = COMMAND_DELETE;
                par->c = NULL;

                connectClient(par);
        } else {
                printf("\nAVISO: Arquivo %s Inexistente!\n", fileUse);
        }
}

void connectSendFile(int consocket, char * client, char * filesSend){

        if (recvInt(consocket)){ // CLIENTE ENCONTRADO

                char * diretorio = (char*) calloc (MAX_LENGHT,sizeof (char));

                int port = recvInt(consocket);
                char * ip = recvString(consocket);

                char * c = strtok(filesSend, " ");

                char * file = strtok(NULL, " ");

                while(file != NULL){

                        strcat(strcpy(diretorio, "./"), client);
                        strcat(strcat(diretorio, "/"), file);

                        sendString(file, consocket);

                        FILE * fi;

                        fi = fopen(diretorio, "r");

                        if (fi != NULL) {
                                fclose(fi);

                                printf("\n\nARQUIVO '%s' SENDO ENVIADO!\n\n", file);

                                pthread_t thread; // inicializar uma nova thread

                                parametros * par = (parametros*) calloc (1,sizeof (parametros));

                                par->port = port;
                                par->ip = ip;
                                par->client = (char*) calloc(100, sizeof(char));
                                strcpy(par->client, diretorio);
                                par->consocket = COMMAND_SEND;
                                par->c = c;
            
                                pthread_create( &thread, NULL, connectClient, par);
                                // connectClient(par);
                                // pthread_join( thread, NULL );

                        } else {
                                printf("\n\nAVISO: ARQUIVO '%s' NÃO ENCONTRADO!\n\n", file);
                        }
                        
                        file = strtok(NULL, " ");

                }

                sendString("NN", consocket);
   
        } else {
                printf("\n\nAVISO: Cliente Inexistente!\n\n");
        }

}

void connectRecvFile(int consocket, char * client, char * filesSend){

        if (recvInt(consocket)){ // CLIENTE ENCONTRADO

                char * diretorio = (char*) calloc (MAX_LENGHT,sizeof (char));

                int port = recvInt(consocket);
                char * ip = recvString(consocket);

                char * c = strtok(filesSend, " ");

                char * file = strtok(NULL, " ");

                while(file != NULL){

                        strcat(strcpy(diretorio, "./"), c);
                        strcat(strcat(diretorio, "/"), file);

                        sendString(file, consocket);

   
                        pthread_t thread; // inicializar uma nova thread

                        parametros * par = (parametros*) calloc (1,sizeof (parametros));

                        par->port = port;
                        par->ip = ip;
                        par->client = (char*) calloc(100, sizeof(char));
                        strcpy(par->client, diretorio);
                        par->consocket = COMMAND_RECV;
                        par->c = client;
        
                        pthread_create( &thread, NULL, connectClient, par);
                        
                        file = strtok(NULL, " ");

                }

                sendString("NN", consocket);
   
        } else {
                printf("\n\nAVISO: Cliente Inexistente!\n\n");
        }

}

void * sendCommands(void * arguments){ // ENVIA COMANDOS AO SERVIDOR GERAL

        struct parametros * par = (struct parametros*) arguments;
        int mysocket = par->consocket;
        char * client = par->client;

        char * filesDelete = (char*) calloc (MAX_LENGHT,sizeof (char));
        char * fileSendDelete = (char*) calloc (MAX_LENGHT,sizeof (char));
        char * filesSend = (char*) calloc (MAX_LENGHT,sizeof (char));
        char * filesRecv = (char*) calloc (MAX_LENGHT,sizeof (char));
        char * copy = (char*) calloc (MAX_LENGHT,sizeof (char));

        while (1) {

                int command;

                printf("\nDigite o comando desejado: \n");
                printf(" Para LISTAR os clientes e arquivos digite 0.\n");
                printf(" Para visualizar o STATUS do servidor digite 1.\n");
                printf(" Para RECEBER arquivos digite 2.\n");
                printf(" Para DELETAR um arquivo digite 3.\n");
                printf(" Para ENVIAR arquivos digite 4.\n");
                printf(" Para SAIR digite 5.\n");


                scanf("%d", &command);

                switch(command){
                        case COMMAND_LIST:
                                system("clear");
                                list(mysocket);
                                break;
                        case COMMAND_STATS:
                                system("clear");
                                sendInt(COMMAND_STATS, mysocket); // ENVIO DO COMMANDO DE STATUS
                                // printf("socket antes statss %d\n", mysocket);
                                status(mysocket);
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
                                // printf("consocket do serv int %d", consocket_serv_client);
                                // close(consocket_serv_client);
                                return EXIT_SUCCESS;
                                break;
                        case COMMAND_DELETE:
                                system("clear");
                                
                                printf("Digite os arquivos que deseja deletar: \n");
                
                                scanf(" %[^\n]", filesDelete);

                                char * file1 = strtok(filesDelete, " ");

                                while(file1 != NULL){
                                        sendInt(COMMAND_DELETE, mysocket); // ENVIO DO COMMANDO DE DELETE FILES
                                        // printf("file: %s\n", file1);
                                        sendString(file1, mysocket); // ENVIO DO ARQUIVO A SER DELETADO
                                        delet(mysocket, file1);
                                        file1 = strtok(NULL, " ");
                                }                                

                                break;
                        case COMMAND_SEND:
                                system("clear");

                                printf("Digite o cliente e os arquivos que deseja enviar: \n");
                
                                scanf(" %[^\n]", filesSend);

                                char * copy = (char*) calloc (MAX_LENGHT,sizeof (char));
                                strcpy(copy, filesSend);

                                // fgets(filesSend, 256, stdin);
                                
                                // printf("filesSend: %s\n\n", filesSend);

                                char * c = strtok(filesSend, " ");

                                /* MANDA PARA O SERVIDOR QUAL O CLIENTE QUE DESEJO ME CONECTAR */

                                sendInt(COMMAND_SEND, mysocket); // ENVIO DO COMMANDO DE SEND
                                
                                sendString(c, mysocket); // ENVIO DO NOME DO CLIENTE
                                
                                connectSendFile(mysocket, client, copy); 
                                // printf("2; %d\n", recvInt(mysocket)); 
                                printf("FIM DO SEND\n");                        
                                break;
                        case COMMAND_RECV:
                                system("clear");

                                printf("Digite o cliente e os arquivos que deseja receber: \n");
                
                                scanf(" %[^\n]", filesRecv);

                                char * copy2 = (char*) calloc (MAX_LENGHT,sizeof (char));
                                strcpy(copy2, filesRecv);

                                printf("RECEBENDO ARQUIVOS ...\n");  

                                // printf("filesRecv: %s\n\n", filesRecv);

                                char * c2 = strtok(filesRecv, " ");

                                /* MANDA PARA O SERVIDOR QUAL O CLIENTE QUE DESEJO ME CONECTAR */

                                sendInt(COMMAND_RECV, mysocket); // ENVIO DO COMMANDO DE RECV
                                
                                sendString(c2, mysocket); // ENVIO DO NOME DO CLIENTE
                                sendString(client, mysocket); // ENVIO DO NOME DO CLIENTE QUE VAI RECEBER O ARQUIVO
                                
                                connectRecvFile(mysocket, client, copy2); 
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
        char * fileUse = (char*) calloc (MAX_LENGHT,sizeof (char));
        char * clientSend = (char*) calloc (MAX_LENGHT,sizeof (char));

        switch(command){

                case COMMAND_DELETE:
                        fileUse = recvString(socket_otherclient);

                        char * diretorio = (char*) calloc (MAX_LENGHT,sizeof (char));
                        strcat(strcpy(diretorio, "./"), client);

                        strcat(strcat(diretorio, "/"), fileUse);

                        remove(diretorio); // REMOVER O ARQUIVO QUE O CLIENTE CONECTADO ME ENVIAR

                        printf("ARQUIVO %s REMOVIDO\n", diretorio);

                        // sendString(fileUse, socket_serv); // ENVIA O ARQUIVO QUE FOI DELETADO PARA O SERVIDOR GERAL
                        
                        break;
                case COMMAND_SEND:
                        puts("");
                        
                        parametros * par = (parametros*) calloc (1,sizeof (parametros));

                        par->consocket = socket_otherclient;
                        par->client = fileUse;

                        pthread_t thread; // inicializar uma nova thread


                        pthread_create( &thread, NULL, recvFileSend, par);

                        // fileUse = recvString(socket_otherclient);
                        // recvFileSend(fileUse, socket_otherclient);

                        break;

                case COMMAND_RECV:
    
                        clientSend = recvString(socket_otherclient);
                        fileUse = recvString(socket_otherclient);
 
                        pthread_t threadR; // inicializar uma nova thread

                        parametros * par2 = (parametros*) calloc (1,sizeof (parametros));

                        par2->consocket = socket_otherclient;
                        par2->client = fileUse;
                        par2->c = clientSend;

                        pthread_create( &threadR, NULL, sendFileRecv, par2);


                        // sendFileRecv(fileUse, clientSend, socket_otherclient);
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

        int consocket_serv_client;

        printf("Server is waiting for connections on port:%d\n", port );

        while (1){
                
                // ESPERANDO POR UMA CONEXÃO
                consocket_serv_client = accept(mysocket, (struct sockaddr *)&dest, &socksize);
                // printf("connection socket:%d\n", consocket_serv_client );
                
                // int * con = &consocket_serv_client;
        
                pthread_t thread; // inicializar uma nova thread

                parametros * par = (parametros*) calloc (1,sizeof (parametros));

                par->consocket = consocket_serv_client;
                par->client = client;
                par->socket_serv = par1->consocket;

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