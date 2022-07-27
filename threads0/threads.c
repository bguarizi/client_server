/*
 * Threads.c
 *
 *  Created on: 21/08/2017
 *      Author: taka
 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

void* print_message_function( void* ptr )//todas as funções do pthread tem q ter esse formato!!!!!!!!!!!!
{
    int* inteiro = (int*) ptr;//convertendo de void* pra int*
    printf("Thread com o argumento:%d\n", *inteiro );

    return (void*) 0;
}

int main( int argc, char** argv )
{
     pthread_t threads[ 25 ];//vetor de threads??

     int argumentos[ 25 ];

     for( int counter = 0; counter < 25; counter++ ){//numera um vetor de argumentos

        argumentos[ counter ] = counter;

     }

    for( int counter = 0; counter < 25; counter++ ){//cria as 25 threads

        pthread_create( &threads[ counter ], NULL, print_message_function, &argumentos[ counter ] );
        //cria cada thread executando a função print_message_function com os argumentos como parametro
    }

    
    void* returnValue = NULL;
    
    //for( int counter = 0; counter < 25; counter++ ){//comentar ciclo

    //    pthread_join( threads[ counter ], returnValue );//espera a thread terminar 

    //}


}



