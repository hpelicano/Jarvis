/*Standar Library*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include<fcntl.h>
#include <dlfcn.h>

/*Network Library*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

/*Local Libraries*/
#include<Servidor2.h>

/*-------------------------------------*/
/*          Entry Point MAIN()         */
/*-------------------------------------*/
int main(int argc, char** argv){

 /*Server section*/ 
 struct sockaddr_in serverAddr;
 socklen_t serverAddrSize;


 /*Misc. Section*/
 int rc = 0;
 int ssfd = 0;

    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    /*------------------------------------------------------------*/
    /* Antes de abrir el canal del socket del server, cargo las   */
    /*  librerias dinamicas.                                      */
    /*------------------------------------------------------------*/
    rc = load_dlls_dinamicas();
    if(rc != 0){
        perror("ERROR al cargar las librerias dinamicas.\n");
        exit(-1);
    }



    /*
      (1)-Creamos el socket del servidor
        - AF_INET: familia IP internet,
        - SOCK_STREAM: orientado a conexion,
        - 0: protocolo de transporte TCP
    */
    ssfd = socket(AF_INET, SOCK_STREAM, 0);
    if(ssfd < 0){
        perror("Error al crear el Socket del Servidor.");
        exit(-1);
    }

    /* (2)-Completamos la estructura de datos del Server */
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY; /*Aceptamos conexiones desde cualquier IP*/

    serverAddrSize = sizeof(serverAddr);

    /* (3)-Realizamos el Bind del socket con la estructura del server */
    rc = bind(ssfd, (const struct sockaddr*) &serverAddr, serverAddrSize );
    if(rc < 0){
        perror("Error Binding.");
        exit(-1);
    }

    /* (4)-Se pone a la escucha el nuevo canal del socket */
    listen(ssfd, 10); /*Maximo de 10 requeste en espera*/


    printf("Proceso a la espera de Request de los Clientes....\n");

    /* (5)-Entramos al procesamiento de los Requests */
    AcceptIncommingConexion(ssfd);
        

 return 0;
}




/*-------------------------------------*/
/* Carga de Modulos de dll's Dinamicas */
/*-------------------------------------*/
int load_dlls_dinamicas(){

 FILE *archivo;
 short i = 0;
 short count = 0;
 short largo = 0;
 short error = false;
 char caracteres[100];
 char msg_err[80];

    /*Inicializo el puntero grlobal*/
    gDllMcb = NULL;

 	archivo = fopen("/home/hpelicano/Roman/C_Programming/Cliente-Servidor/Servidor/src/modulos.txt","r");
 	
 	if (archivo == NULL){
        exit(1);
    }else{
 	    /*Cuento la cantidad de modulos Activos para dimensionar la memioria*/
        while (feof(archivo) == 0){
            fgets(caracteres,100,archivo);
            if(strncmp(caracteres, "#",1)){
                count++;
            }
 	    }
        
        /*Guardo en la global la cantidad de modulos activos*/
        count_dlls_mcb = count;

        /*Dimensiono el espacio de memoria dinamica*/
        gDllMcb = (DLL_MCB*)malloc( count * sizeof(DLL_MCB));
        if(gDllMcb == NULL){
            perror("ERROR al asignar memoria global de liberrias dinamicas.\n");
            exit(-1);
        }

        /*Inicializo la memoria con ceros binarios*/
        memset(gDllMcb, '\0', count * sizeof(DLL_MCB));

        /*Me vuelvo al inicio del archivo*/
        fseek(archivo,0L, SEEK_SET);

        /*Cargo los Path_File de cada modulo en la memoria global*/
        printf("----------------------------\n");
        printf("Cargando Modulos Dinamicos: \n");
        printf("----------------------------\n");
 	    
        for (i=0;i<count;i++){
            
            fgets(caracteres,100,archivo);
            
            if(!strncmp(caracteres," ",1)){
                break;
            }else{
                if(strncmp(caracteres,"#",1)){
                    largo = strlen(caracteres);
                    strncpy(gDllMcb[i].name, caracteres, 5); 
                    strncpy(gDllMcb[i].moduleName, &caracteres[6], (largo - 7));
                }
            }
 	    }


        /*Comenzamos el proceso de linkeo dinamico con los modulos y sus metodos*/
        for (i=0;i<count;i++){
            /*[A]- Obtengo la dlHandle de la libreria */
            gDllMcb[i].dllHandle = dlopen(gDllMcb[i].moduleName, RTLD_NOLOAD);

            if(gDllMcb[i].dllHandle == 0){
                gDllMcb[i].dllHandle = dlopen(gDllMcb[i].moduleName, RTLD_LAZY);
            }

            if (!gDllMcb[i].dllHandle){
                fprintf (stderr, "%s\n", dlerror());
                printf("Error al obtener el Handle de las Dlls. [%s]", gDllMcb[i].moduleName);
                return 1;
            }

            /*[B]- Obtengo el Address de la DLLInfo */
            gDllMcb[i].dllInfo = (short (*)(char*))dlsym(gDllMcb[i].dllHandle, "DLLInfo");
            if (!gDllMcb[i].dllInfo){
                fprintf (stderr, "%s\n", dlerror());
                perror("Error al obtener Address de DLLInfo");
                error = true;
            }

            /*[C]- Obtengo el Address de la DLLProcesarRequest */
            gDllMcb[i].dllProcesarRequest = 
                (short (*)(char* recv_msg, ClientAccepted* p_client))dlsym(gDllMcb[i].dllHandle, "DLLProcesarReq");
            if (!gDllMcb[i].dllProcesarRequest){
                fprintf (stderr, "%s\n", dlerror());
                perror("Error al obtener Address de DLLProcesarRequest");
                error = true;
            }

            if (error){
                memset(msg_err,' ', sizeof(msg_err));
                strcpy(msg_err, "Verificar el archivo 'MODULOS' para el registro: ");
                strncat(msg_err, gDllMcb[i].moduleName, 50 );
                printf("[%s]", msg_err);

                dlclose(gDllMcb[i].dllHandle);
                exit(-1);
            }

            printf("\nSe ha cargado el Modulo: [%s]", gDllMcb[i].moduleName);

        }/*Fin del FOR()*/

        printf("\n\n");
        printf("-------------------------------------\n");
        printf("Carga de Libreria Dinamicas Exitosas.\n");
        printf("-------------------------------------\n");
        printf("\n\n");

    }
    
    fclose(archivo);
    return 0;
}


/*-------------------------------------*/
/* Procesamiento de nuevas conexiones  */
/*-------------------------------------*/
void AcceptIncommingConexion(int ssfd){
 
 struct sockaddr_in clientAddr;
 socklen_t clientAddrSize = sizeof(clientAddr);
 ClientAccepted* clientAccepted;
 int csfd;

    clientAccepted = NULL;

    while(true){

        /*creamos una conexion con el cliente para transferencia de datos*/
        csfd = accept(ssfd,(struct sockaddr*)&clientAddr, &clientAddrSize);
        if(csfd < 0 ){
            perror("Error al generar un nuevo Socket con el Cliente");
            exit(-1);
        }

        /*Unifico la info del cliente para pasarlo como argumento del HandlerRequest()*/
        clientAccepted = (ClientAccepted*)malloc(sizeof(ClientAccepted));
        clientAccepted->socketClient = csfd;
        clientAccepted->addr = &clientAddr;    
        clientAccepted->bussy = 0;

        /*Gestino un nuevo Hilo para comenzar a procesar los Request*/
        ManagerThreadHandler(csfd, clientAccepted);

    }

}

/*-------------------------------------*/
/* Manager Handler Clients Conexions   */
/*-------------------------------------*/
void ManagerThreadHandler(int p_csfd, ClientAccepted* p_clientAccepted){

 pthread_t newClient;
 pthread_attr_t clientAttr;


    /*(1)-Creamos la nueva instacia de socket para el cliente */
    pthread_attr_init(&clientAttr);

    /*(2)-Modo Detached para que no se quede esperando respuesta*/
    pthread_attr_setdetachstate(&clientAttr, PTHREAD_CREATE_DETACHED);

    /*(3)-Creamos el Socket y lo enviamos al Handler()*/
    pthread_create(&newClient, &clientAttr, HandlerRequest, (void*)p_clientAccepted);

    /*(4)-Ya no usamos los Attr's*/
    pthread_attr_destroy(&clientAttr);

}

/*-------------------------------------------------*/
/* Handler Requests                                */
/* Evaluo el mensaje y determino a donde derivarlo */
/*-------------------------------------------------*/
void* HandlerRequest(void* p_client){

 ClientAccepted* cliente = (ClientAccepted*)p_client;
 int i = 0;
 int encontrado = 0;
 int bytes_recibidos = 0;
 char mensaje[MSG_SIZE_MAX];
 char hdr_msg[4+1];


    /*Inicializo en ceros el vector*/
    bzero(mensaje, MSG_SIZE_MAX);

    bytes_recibidos = recv(cliente->socketClient, mensaje, MSG_SIZE_MAX, 0);
    if(bytes_recibidos == 0){
        perror("No se recibio el mensaje del cliente.");
        return NULL;
    }

    strncpy(hdr_msg, mensaje, 4);
    hdr_msg[4] = '\0';   


    for(i=0;i<count_dlls_mcb;i++){
        if(!strncmp(gDllMcb[i].name, hdr_msg, 4)){
            encontrado = 1;
            gDllMcb[i].dllProcesarRequest(mensaje, cliente);
        }

        if(encontrado){
            break;
        }
    }




    return NULL;

}