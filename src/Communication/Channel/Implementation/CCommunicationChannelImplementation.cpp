#include "inc/Communication/Channel/Implementation/CCommunicationChannelImplementation.h"

void* internalMsgRecvThread_func(void* pArgv);
void* externalMsgTranThread_func(void* pArgv);

CCommunicationChannelImplementation::CCommunicationChannelImplementation(CChannelMessageConsumer* iConsumer)
{
    consumer = iConsumer;
}

TIMM_OSAL_ERRORTYPE CCommunicationChannelImplementation::MsgReceive(TIMM_OSAL_PTR* pCurMessage)
{
    TIMM_OSAL_ERRORTYPE error = TIMM_OSAL_ERR_NONE;
    systemMessage_t* message;
    TIMM_OSAL_U32 nMessageSize = 0;

    // Allocate space for message header
    message = (systemMessage_t*)TIMM_OSAL_MallocExtn(DEFAULT_MESSAGE_SIZE, TIMM_OSAL_TRUE, 32, 
        TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    //printf("internalMsgRecvThread: Allocate message\n");
    if (!message)
    {
        printf("Cant allocate %d bytes form message header\n", DEFAULT_MESSAGE_SIZE);
        return TIMM_OSAL_ERR_ALLOC;
    }

    *(systemMessage_t **)pCurMessage = message;
    message->pPayload = NULL;
    //TIMM_OSAL_ReadFromPipe doesn't support timeout
    // printf("internalMsgRecvThread: Read from Pipe!\n");
    error = TIMM_OSAL_ReadFromPipe(internalMsgPipe, message, DEFAULT_MESSAGE_SIZE, &nMessageSize,
        TIMM_OSAL_SUSPEND);//inifinite timeout
    if (TIMM_OSAL_ERR_NONE != error)
    {
        TIMM_OSAL_Free(message);
        message = NULL;
        printf("Can't read from message pipe\n");
        return error;
    } else if (nMessageSize < sizeof(systemMessage_t))
    {
        TIMM_OSAL_Free(message);
        message = NULL;
        printf("Read data is less than message header size\n");
        return TIMM_OSAL_ERR_MSG_SIZE_MISMATCH;
    }

    return TIMM_OSAL_ERR_NONE;
}

TIMM_OSAL_ERRORTYPE CCommunicationChannelImplementation::WriteAssembledMsg(systemMessage_t* message)
{   //send message to internalMsgPipe
    return TIMM_OSAL_WriteToPipe(internalMsgPipe, message, sizeof(systemMessage_t), TIMM_OSAL_SUSPEND);
}

TIMM_OSAL_ERRORTYPE CCommunicationChannelImplementation::Write(systemMessage_t* message)
{
    return tcpComm->Send(message);
}

TIMM_OSAL_ERRORTYPE CCommunicationChannelImplementation::Read(systemMessage_t* message)
{
    return tcpComm->Receive(message);
}

TIMM_OSAL_ERRORTYPE CCommunicationChannelImplementation::Init()
{
    TIMM_OSAL_ERRORTYPE error = TIMM_OSAL_ERR_NONE;

    Exit = false;

    //Allocate new object
    tcpComm = new CCommunicationMediaTcp();

	//Init TCP Connetction
    error = tcpComm->Init();
    if (TIMM_OSAL_ERR_NONE != error)
    {
        MMS_IL_PRINT("Communication channel not inited. Restart!Error - 0x%08X\n", error);
        tcpComm->Deinit();
        return TIMM_OSAL_ERR_NOT_READY;
    }

  /*  error = TIMM_OSAL_SemaphoreCreate(&COMM_Close_Semaphore, 0);
	//Init Semaphore COMM_Close_Semaphore
    if (TIMM_OSAL_ERR_NONE != error)
    {
        printf("TIMM_OSAL_SemaphoreCreate failed! Error - 0x%08X\n", error);
    }*/

    // Create Comunication Pipe
    error = TIMM_OSAL_CreatePipe(&internalMsgPipe, DEFAULT_PIPE_SIZE, DEFAULT_MESSAGE_SIZE, 1);
    if (TIMM_OSAL_ERR_NONE != error)
    {
        MMS_IL_PRINT("TIMM_OSAL_CreatePipe failed! Error - 0x%08X\n", error);
    }
	//Create new thread for internalMsgRecvThread_func
    error = pthread_create(&internalMsgRecvThread, NULL, internalMsgRecvThread_func, (CCommunicationChannel*)this);
    if(error)
    {
        MMS_IL_PRINT("TIMM_OSAL_CreateTask failed! Error - 0x%08X\n", error);
    }else error =TIMM_OSAL_ERR_NONE;
    return error;
}

void CCommunicationChannelImplementation::Deinit()
{
    TIMM_OSAL_ERRORTYPE error = TIMM_OSAL_ERR_NONE;
    
    // Delete Comunication Pipes
    error =  TIMM_OSAL_DeletePipe(internalMsgPipe);
    if (TIMM_OSAL_ERR_NONE !=  error)
    {   
        MMS_IL_PRINT("TIMM_OSAL_DeletePipe failed! Error - 0x%08X\n", error);
    }
     //printf("Main thread: Pipe Deleted\n");
    /*error = TIMM_OSAL_SemaphoreDelete(COMM_Close_Semaphore);
    if (TIMM_OSAL_ERR_NONE != error)
    {
        MMS_IL_PRINT("TIMM_OSAL_SemaphoreDelete failed! Error - 0x%08X\n", error);
    }*/
    error = tcpComm->Deinit();
    if (TIMM_OSAL_ERR_NONE != error)
    {
        MMS_IL_PRINT("Channel deinit failed! Error - 0x%08X\n", error);
    }
    //wait for externalMsgTranThread to finish before continuing
    pthread_join(externalMsgTranThread, NULL);
    printf("Main thread: externalMsgTranThread is finished! Restarting..... \n");

    delete(tcpComm);
}

void CCommunicationChannelImplementation::Start()
{
    int error = 0;
	
    //Create new htread for externalMsgTranThread_func
    error = pthread_create(&externalMsgTranThread, NULL, externalMsgTranThread_func, (CCommunicationChannel*)this);
    if(error)
	{
        MMS_IL_PRINT("TIMM_OSAL_CreateTask failed! Error - 0x%08X\n", error);
    }
}

void CCommunicationChannelImplementation::Stop()
{ //TODO
    systemMessage_t message;
    printf("Closing Connection!\n");
    Exit = true;
     //Send dummy msg to pipe
    message.nId = -1;//non existing message
    message.nStatus = 0;
    message.nPayloadSize = 0;
    message.pPayload = NULL;
    TIMM_OSAL_ERRORTYPE error = WriteAssembledMsg(&message);
    if (TIMM_OSAL_ERR_NONE != error)
    {
      MMS_IL_PRINT("TIMM_OSAL_WriteToPipe failed! Error - 0x%08X\n", error);
      printf("internalMsgRecvThread will not be stopped!\n");
      printf("Stop failed!\n");
    }
   // TIMM_OSAL_SemaphoreRelease(COMM_Close_Semaphore);
}

void CCommunicationChannelImplementation::WaitOnCommObject()
{
    //Wait on Semaphore
   /* if (TIMM_OSAL_ERR_NONE != TIMM_OSAL_SemaphoreObtain(COMM_Close_Semaphore, TIMM_OSAL_SUSPEND))
    {
        printf("TIMM_OSAL_SemaphoreObtain failed\n");
    }*/
    //wait for externalMsgTranThread to finish before deiniting
    pthread_join(internalMsgRecvThread, NULL);
    printf("Main thread: internalMsgRecvThread is finished! \n");
}

void* internalMsgRecvThread_func(void* pArgv)
{//Runs on different thread, which is created in during TCP connection init
    CCommunicationChannelImplementation* msgChan = (CCommunicationChannelImplementation*)pArgv;
    systemMessage_t* pReceiveMessage;
    TIMM_OSAL_ERRORTYPE error = TIMM_OSAL_ERR_NONE;

    while(!msgChan->isClosing() )
    {
        //Recieve message from pipe
        error = msgChan->MsgReceive( (void**)&pReceiveMessage);
        if (TIMM_OSAL_ERR_NONE ==  error)
        {
         // Call MessageEventHandler, Process mesage, Send ACK
         msgChan->consumer->MessageEventHandler(pReceiveMessage);
        } else MMS_IL_PRINT("TIMM_OSAL_ReadFromPipe failed! Error - 0x%08X\n", error);

		    //Free message buffer
            if (pReceiveMessage)
            {
                if (pReceiveMessage->pPayload)
                {
                    TIMM_OSAL_Free(pReceiveMessage->pPayload);
                    pReceiveMessage->pPayload = NULL;
                }
                TIMM_OSAL_Free(pReceiveMessage);
                pReceiveMessage = NULL;
            }// if (pReceiveMessage)

    }//end while(!msgChan->isClosing() )
    printf("internalMsgRecvThread: Channel is closed!\n");
    return NULL;
}

void* externalMsgTranThread_func(void* pArgv)
{
    CCommunicationChannelImplementation* msgChan = (CCommunicationChannelImplementation*)pArgv;

    systemMessage_t* pMessage;
    TIMM_OSAL_ERRORTYPE error = TIMM_OSAL_ERR_NONE;

    while(!msgChan->isClosing())
    { //loop messages are revieved from TCP
        // Allocate space for message header
        //pMeesage is freed in internalMsgRecvThread_func after
        //it has been received and read
        pMessage = (systemMessage_t*)malloc(sizeof(systemMessage_t));
		//Receive message from TCP
        //printf("externalMsgTranThread: Reading Message!\n");
        if(TIMM_OSAL_ERR_NONE == msgChan->Read(pMessage) )
        {
           //Write message to pipe
           // printf("externalMsgTranThread: Write mgs t pipe!\n");
            error = msgChan->WriteAssembledMsg(pMessage);
            if (TIMM_OSAL_ERR_NONE != error)
            {
               MMS_IL_PRINT("TIMM_OSAL_WriteToPipe failed! Error - 0x%08X\n", error);
            }
        }
        else if (!msgChan->isClosing())
        {
            printf("externalMsgTranThread: STOP! \n");
            msgChan->Stop();
        }
    }
    printf("externalMsgTranThread: Channel is closed!\n");


    return NULL;
}