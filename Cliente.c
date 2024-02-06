/*Standar Library*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

/*Network Library*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>



int main(){
 struct sockaddr_in serverAddr;

 int ssfd;
 int rc = 0;
 int bytes_enviados = 0;
 int bytes_recibidos = 0;

 char mensaje[100];
 char buffer[100];

    /*(1)-Creacion del socket para el servidor*/
    ssfd = socket(AF_INET, SOCK_STREAM, 0);
    if(ssfd < 0){
        puts("Error al crear el socket del servidor.\n");
        exit(-1);
    }   

    /*Inicializo en cero la estructura*/
    bzero((char*)&serverAddr, sizeof(serverAddr));

    /*(2)-Configurar la estructura de Address del server*/
    serverAddr.sin_family      = AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&serverAddr.sin_addr.s_addr);
    serverAddr.sin_port        = htons(5555);

    rc = connect(ssfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in));
    if(rc < 0){
        printf("Error en la Conexion con el Servidor.\n");
        exit(-1);
    }
    
    printf("Conexión exitosa con el servidor.\n\n");

    bzero(mensaje, sizeof(mensaje));
    bzero(buffer, sizeof(buffer));

    strcpy(mensaje, "AUDIT=>Te saludo desde el Cliente.");

    bytes_enviados = send(ssfd, mensaje, sizeof(mensaje), 0);
    if (bytes_enviados == 0)
        printf("Nada para enviar al server.\n");



    while( (bytes_recibidos = recv(ssfd, buffer, sizeof(buffer), 0)) > 0 ){

            printf("[ %s ].\n", buffer);

    }


    printf("[ ----> El server Termino la conexion <---- ].\n");

    close(ssfd);

    return 0;
}