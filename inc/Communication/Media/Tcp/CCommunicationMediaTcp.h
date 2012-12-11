#ifndef __CCommunicationMediaTcp__H__
#define __CCommunicationMediaTcp__H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "inc/Communication/Media/CCommunicationMedia.h"
#include <semaphore.h>
#include "inc/lib_comm.h"

#define TCPIP_PORT "7777"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

class CCommunicationMediaTcp : public CCommunicationMedia
{

public:
	//Inits semaphore mutex and calls TCPIP_Messaging_Init_Function
    virtual TIMM_OSAL_ERRORTYPE Init();
    virtual TIMM_OSAL_ERRORTYPE Deinit();
    virtual TIMM_OSAL_ERRORTYPE Receive(void* pCurMessage);
    virtual TIMM_OSAL_ERRORTYPE Send(void* pMessage);
    ~CCommunicationMediaTcp();

private:
    int fd;
    int sockfd;

    TIMM_OSAL_ERRORTYPE TCPIP_ReceiveNumBytes(void* pBuffer, unsigned int nNumBytes);
    TIMM_OSAL_ERRORTYPE TCPIP_WriteNumBytes(void* pBuffer, unsigned int nNumBytes);
	//Initialize TCP connetction
    TIMM_OSAL_ERRORTYPE TCPIP_Messaging_Init_Function();
    TIMM_OSAL_ERRORTYPE TCPIP_Messaging_Deinit_Function();
	sem_t mutex;
};

#endif