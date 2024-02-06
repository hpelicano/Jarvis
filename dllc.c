#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"dllh.h"
#include <sys/socket.h>

short DLLInfo(char* p_id){

    printf("Esta es el metodo de DLLInfo. [%s] .\n", p_id);

    return 0;
}


short DLLProcesarReq(char* recv_msg, ClientAccepted* p_client){

  size_t bytes_leidos;
  FILE* fd;
  char buffer[100];

  
  /* Abrimos u archivo*/
  fd = fopen("/home/hpelicano/Roman/C_Programming/Cliente-Servidor/archivo","r");
  if(fd == NULL){
    perror("ERROR al abrir el archivo de prueba.");
  }else{

    while(!feof(fd)){
      bytes_leidos = fread(buffer, sizeof(char), sizeof(buffer), fd);
      send(p_client->socketClient, buffer, sizeof(buffer),0);
    }

  }


  /*Cuando termino, cierro el socket con el cliente*/
  close(p_client->socketClient);

  /*Libero el espacio de memoria asignado para el cliente*/
  free(p_client);

  return 0;
}
