
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include "biblioteca.h"


void saveFiles(char * name_c, char * n_file, clients_list *cl){ // Função que adiciona os arquivos recebidos em uma estrutura de dados

    client * aux = cl->first;

    while(strcmp(aux->name, name_c) != 0) {
        aux = aux->next;
    }

    file *aux2 = aux->files;
    
    if (aux2 == NULL) {

        file *new_f = (file*) calloc (1,sizeof (file));
        new_f -> name = n_file;
        new_f -> next = NULL;
        aux->files = new_f;
    
    } else {

        while(aux2->next != NULL) {
            aux2 = aux2->next;
        }
        
        file *new_f = (file*) calloc (1,sizeof (file));
        new_f -> name = n_file;
        new_f -> next = NULL;
    
        aux2->next = new_f;
    
    }  

}

void listFiles(int consocket, clients_list *cl) { // Função que lista os clintes e arquivos salvos no servidor

    int count = 0;

    client * auxT = cl->first;
    client * aux = cl->first;

    while (auxT != NULL){

        count++;

        auxT = auxT->next;
    }

    sendInt(count, consocket); // ENVIA A QUANTIDADE DE CLIENTES NA LISTA
    
    while (aux != NULL){

        sendString(aux->name, consocket);

        sendInt(aux->numFiles, consocket); // ENVIAR O NUMERO DE ARQUIVOS QUE EU TENHO NO CLIENTE
        
        file *aux2 = aux->files;
        
        while(aux2 != NULL){
            printf("%s\n", aux2->name);
            sendString(aux2->name, consocket);

            aux2 = aux2 -> next;
        }

        printf("\n\n");

        printf("FIM LISTAR!\n");
        
        aux = aux->next;
    }
        
    

}

void recvFilesNames (int command, int mysocket, clients_list *cl, char* client_name){ // Função que recebe os arquivos de um cliente

    for(int i = 0; i < command; i ++) {
        char * recvFile = recvString(mysocket);
        saveFiles(client_name, recvFile, cl);
        
    }
    
}

void recvFiles (int consocket, clients_list *cl, char * ip, int count){

    client *c = (client*) calloc (1,sizeof (client));

    int port;

    if (count == 0) {

        port = recvInt(consocket);

        c->port = port;
        c->ip = ip;
        
    }

    int numarquivos = recvInt(consocket); // Vai receber o total de arquivos que o diretorio possui
    
    char * client_name = recvString(consocket);

   
    c->name = client_name;
    c->next = NULL;
    c->files = NULL;
    c->numFiles = numarquivos;


    // INSERE O NOVO CLIENTE NA LISTA DE ADJACENCIA

    if (cl->first == NULL){
        cl->first = c;
    } else {
        client *aux = cl->first;    
        
        while(aux->next != NULL){
            aux = aux->next;
        }

        aux->next = c;
    }

    cl->last = c;

    recvFilesNames(numarquivos, consocket, cl, client_name);
}

void removeClient(char* c, clients_list *cl){ // Elaborar remoção  para quando um cliente se desconectar

    client * aux = cl->first;
    client * aux2;

    if (strcmp(aux->name, c) == 0){
        cl->first = aux->next;
    } else {
        while(aux != NULL) {

            if (strcmp(aux->name, c) == 0) {
                // aux(free);
                if (strcmp(cl->last->name, aux->name) == 0){
                    aux2->next = NULL;
                    cl->last = aux2;
                } else {
                     aux2->next = aux->next;
                }

                break;
            }

            aux2 = aux;

            aux = aux->next;
        }
    }



}

void findFile(char * nameFile, clients_list *cl, int consocket){

    client * aux = cl->first;
    int count = 0;

    file * auxfl = NULL;
    file * auxfl1 = NULL;

    while(aux != NULL){

        auxfl = aux->files;
        auxfl1 = NULL;

        while(auxfl != NULL){

            if (strcmp(auxfl->name, nameFile) == 0) {
                count = 1;
                sendInt(1, consocket); // Envia um int para dizer que achou o arquivo

                sendInt(aux->port, consocket); // PORTA
                sendString(aux->ip, consocket); // IP
    
                break;
                //sendStrig(); // ENVI
            }

            auxfl1 = auxfl;

            auxfl = auxfl -> next;

        }

        if(count == 1)
            break;

        aux = aux->next;

    }

    if (count == 0) {
        
        sendInt(0, consocket); // Envia um int para dizer que não achou o arquivo

    } else { // TIRA O ARQUIVO DA LISTA ENCADEADA
        
        if (auxfl1 == NULL) { // SE FOR O PRIMEIRO DA LISTA
            printf("FIRST LISTA AAAAAAAAA\n"); //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
            aux->files = auxfl -> next;
            free(auxfl);
        } else {
            if (auxfl->next != NULL) {
                printf("file1: %s e file2: %s\n", auxfl1->name, auxfl->next->name);
            }

            auxfl1->next = auxfl->next;
            free(auxfl);
        }

        aux->numFiles--;
    }
}

void * process_commands(void *recvparametros){

    struct parametros * par = (struct parametros*) recvparametros;

    int consocket = par->consocket;
    clients_list *cl = par->cl;

    int count = 0; // MUDAR O CONTADOR SÓ PARA SABER QUANDO FOI ENVIADA PELA PRIMEIRA VEZ OS ARQUIVOS, ASSIM NÃO TENTAREI PEGAR NOVAMENTE A PORTA E O IP 

    while(consocket)
    {

        int command = recvInt(consocket); // Vai receber o respectivo comando que o cliente deseja fazer

        if (command == COMMAND_LIST){ // LISTAR

            listFiles(consocket, cl);

        } else if (command == COMMAND_STATS){ //STATS

        } else if (command == SENDFILENAMELIST){ // SENDFILES
            
            recvFiles(consocket, cl, par->ip, count);
            count = 1;

        } else if (command == COMMAND_EXIT){ // EXIT
            char * client = recvString(consocket);
            removeClient(client, cl);
            printf("REMOVIDO!\n");
            close(consocket);
        } else if (command == COMMAND_DELETE){

            char * fileDelete;
            
            fileDelete = recvString(consocket);

            findFile(fileDelete, cl, consocket);

            // printf("FILE NAME DELETED IS %s\n", recvString(consocket));

        }

    }

}

int main(int argc, char *argv[])
{

    if( argc != 2 ){ 

        printf("USAGE: server port_number\n");

        return EXIT_FAILURE;

    }


    srand( time( NULL ));

    struct sockaddr_in dest; /* socket info about the machine connecting to us */
    struct sockaddr_in serv; /* socket info about our server */
    int mysocket;            /* socket used to listen for incoming connections */
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
    serv.sin_port = htons( atoi( argv[ 1 ] ) );           /* set the server port number */

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    /* bind serv information to mysocket */
    bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    /* start listening, allowing a queue of up to 1 pending connection */
    listen(mysocket, 1);


    printf("Server is waiting for connections on port:%s\n", argv[ 1 ] );

     // Criando a minha lista de clientes 

    clients_list *cl = (clients_list*) calloc (1,sizeof (clients_list));
    cl->first = NULL;
    cl->last = NULL;

    int consocket;

    int numClients = 0;

    while (1){
        
        // ESPERANDO POR UMA CONEXÃO
        consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);

        char * clientIP = inet_ntoa(dest.sin_addr); // GUARDA O IP DO MEU SERVIDOR
        numClients++;
        pthread_t thread; // inicializar uma nova thread

        parametros * par = (parametros*) calloc (1,sizeof (parametros));

        par->consocket = consocket;
        par->cl = cl;
        par->ip = clientIP;

        pthread_create( &thread, NULL, process_commands, par);

        //pthread_join(thread, NULL);
        
    }
    

    close(consocket);
    return EXIT_SUCCESS;
}
