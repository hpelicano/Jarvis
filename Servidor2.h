#ifndef _Servidor2_h_
#define _Servidor2_h_

#define MSG_SIZE_MAX 8192
#define BUF_SIZE_MAX 8192

#define PORT 5555

/***********************************/
/* Tipo de ruteo interno           */
/***********************************/
#define AUDIT      1001
#define TERMINAL   1002
#define ENFORM     1003
#define LOGGER     1004
#define PTLF       1005

#define REQUEST    0200
#define RESPONSE   0210
#define RVSL_REQ   0420
#define RVSL_RESP  0430
#define COMPLITION 0220
#define ADVISE     0230



/***********************************/
/* Nodo: Elemento de una Lista     */
/***********************************/
typedef struct _ClientAccepted{
    int socketClient;
    struct sockaddr_in* addr;
    int bussy; 
}ClientAccepted;

/******************************/
/* DLL's Memory Control Block */
/******************************/
typedef struct _dll_mcb{
    char   name[5];
    char   status[1];
    char   moduleName[100];
    void*  dllHandle;
    short (*dllInfo)(char* p_id);
    short (*dllProcesarRequest)(char* recv_msg, ClientAccepted* p_client);
}DLL_MCB;

DLL_MCB* gDllMcb;
short count_dlls_mcb;



/*********************************************************/
/*                Prototipo de funciones                 */
/*********************************************************/
int load_dlls_dinamicas();
void AcceptIncommingConexion(int ssfd);
void ManagerThreadHandler(int p_csfd, ClientAccepted* p_clientAccepted);
void* HandlerRequest(void* p_client);


#endif