#ifndef __CCommunicationChannelImplementation__H__
#define __CCommunicationChannelImplementation__H__

#include <timm_osal_interfaces.h>
#include <timm_osal_task.h>
#include <timm_osal_trace.h>
#include <stdio.h>
#include <pthread.h>
#include "inc/Communication/Channel/CCommunicationChannel.h"
#include "inc/Communication/Channel/CChannelMessageConsumer.h"
#include "inc/Communication/Media/Tcp/CCommunicationMediaTcp.h"

#define DEFAULT_STACK_SIZE      (8192)
#define DEFAULT_TASK_PRIORITY   (5)
#define DEFAULT_MESSAGE_SIZE    (128)
#define DEFAULT_PIPE_SIZE       (32 * DEFAULT_MESSAGE_SIZE)

class CCommunicationChannelImplementation : public CCommunicationChannel
{
    friend void* internalMsgRecvThread_func(void* pArgv);
	//Receive message from TCP and release COMM_Close_Semaphore
    friend void* externalMsgTranThread_func(void* pArgv);

public:
	//Initialize  consumer of type CChannelMessageConsumer*
    CCommunicationChannelImplementation(CChannelMessageConsumer* iConsumer);
    TIMM_OSAL_ERRORTYPE Init();
    void Deinit();
    void Start();
    void Stop();
    void WaitOnCommObject();
    TIMM_OSAL_ERRORTYPE Write(systemMessage_t* message);

private:
    CCommunicationMedia* tcpComm;

    pthread_t internalMsgRecvThread;
    pthread_t externalMsgTranThread;
    TIMM_OSAL_PTR COMM_Close_Semaphore;

    TIMM_OSAL_ERRORTYPE Read(systemMessage_t* message);
    TIMM_OSAL_ERRORTYPE MsgReceive(TIMM_OSAL_PTR* pCurMessage);
    TIMM_OSAL_ERRORTYPE WriteAssembledMsg(systemMessage_t* message);

    CChannelMessageConsumer* consumer;
    void* internalMsgPipe;
};

#endif