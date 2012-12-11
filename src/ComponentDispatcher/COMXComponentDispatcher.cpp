#include "inc/ComponentDispatcher/COMXComponentDispatcher.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandle.h"
#include "inc/ComponentDispatcher/MessageHandles/CComponentDispatcherMessageHandleFactory.h"

COMXComponentDispatcher::COMXComponentDispatcher()
{
	msgHandlesHold.clear();
#ifdef DVP_ENABLE
    DVPComponentTable.clear();
#endif
    I2C_device = 0;
}

void COMXComponentDispatcher::AddMessageHandleToHold(int msgId)
{//add messageHandle to msgHandlesHold vector
	msgHandlesHold[msgId] = CComponentDispatcherMessageHandleFactory::CreateMessageHandle(this, msgId);
}

void COMXComponentDispatcher::Start()
{
	//Create CComponentDispatcherCommandHandle object
    AddMessageHandleToHold(OMX_COMMAND_MESSAGE);
    //Create SYS_TCP_DISCONNECT object
    AddMessageHandleToHold(SYS_TCP_DISCONNECT);
	//Create CComponentDispatcherDSSConfigHandle object
    AddMessageHandleToHold(SYS_DSS_CFG_MSG);
	//Create CComponentDispatcherDSSInitHandle object
    AddMessageHandleToHold(SYS_DSS_INIT_MSG);
    //Create CComponentDispatcherEmptyThisBufferHandle object
	AddMessageHandleToHold(OMX_PORT_EMPTYTHIS_BUFFER);
    //Create CComponentDispatcherFillThisBufferHandle object
	AddMessageHandleToHold(OMX_PORT_FILLTHIS_BUFFER);
    //Create CComponentDispatcherGetConfigHandle object
	AddMessageHandleToHold(OMX_GET_CFG_MESSAGE);
    //Create CComponentDispatcherGetHandleHandle object
	AddMessageHandleToHold(OMX_GET_HANDLE_MESSAGE);
    //Create CComponentDispatcherGetILClientVersionHandle object
	AddMessageHandleToHold(SYS_GET_IL_CLIENT_VER);
    //Create CComponentDispatcherGetOMXVersionHandle object
	AddMessageHandleToHold(OMX_GET_OMX_VERSION);
    //Create CComponentDispatcherGetParamHandle object
	AddMessageHandleToHold(OMX_GET_PARAM_MESSAGE);
    //Create CComponentDispatcherGetPortInfoHandle object
	AddMessageHandleToHold(OMX_GET_COMPONENT_PORTS_MESSAGE);
    //Create CComponentDispatcherMarshalFileCopyHandle object
	AddMessageHandleToHold(SYS_MARSHAL_MEM_FILE_COPY);
    //Create CComponentDispatcherMarshalMemAllocHandle object
	AddMessageHandleToHold(SYS_MARSHAL_MEM_ALLOC);
    //Create CComponentDispatcherMarshalMemAllocTypeHandle object
	AddMessageHandleToHold(SYS_MARSHAL_MEM_ALLOC_TYPE);
	//Create CComponentDispatcherMarshalMemCopyHandle object
    AddMessageHandleToHold(SYS_MARSHAL_MEM_COPY);
    //Create CComponentDispatcherMarshalMemDumpHandle object
	AddMessageHandleToHold(SYS_MARSHAL_MEM_DUMP);
	//Create CComponentDispatcherMarshalMemFreeHandle object
    AddMessageHandleToHold(SYS_MARSHAL_MEM_FREE);
	//Create CComponentDispatcherMarshalMemReadHandle object
    AddMessageHandleToHold(SYS_MARSHAL_MEM_READ);
	//Create CComponentDispatcherPortAllocateBufferHandle object
    AddMessageHandleToHold(OMX_ALLOCATE_PORTBUFFERS_MESSAGE);
	//Create CComponentDispatcherPortAutoBuffCycleHandle object
    AddMessageHandleToHold(SYS_AUTOBUF_MANAGEMENT);
	//Create CComponentDispatcherPortConfigAllocTypeHandle object
    AddMessageHandleToHold(SYS_PORT_ALLOC_TYPE);
	//Create CComponentDispatcherPortLinkHandle object
    AddMessageHandleToHold(OMX_LINK_COMPONENTS_MESSAGE);
	//Create CComponentDispatcherPortSetUpServiceHandle object
    AddMessageHandleToHold(SYS_REG_PORT_SERVICES);
	//Create NULL
    AddMessageHandleToHold(OMX_PORT_USEMRSCTX_BUFFER);
	//Create CComponentDispatcherReleaseHandleHandle object
    AddMessageHandleToHold(OMX_RELEASE_HANDLE_MESSAGE);
	//Create CComponentDispatcherSetConfigHandle object
    AddMessageHandleToHold(OMX_CFG_MESSAGE);
	//Create CComponentDispatcherSetParamHandle object
    AddMessageHandleToHold(OMX_PARAM_MESSAGE);
	//Create CComponentDispatcherSetPortBufferCountHandle object
    AddMessageHandleToHold(OMX_SET_ACTUAL_BUFFERS_ON_PORT);
	//Create CComponentDispatcherStreamerSetNamePrefixHandle object
    AddMessageHandleToHold(SYS_CONF_PORT_SERVICE);
    //Create CComponentDispatcherStreamerSetNamePrefixHandle object
    AddMessageHandleToHold(I2C_GET_HANDLE_MESSAGE);
    //Create CComponentDispatcherStreamerSetNamePrefixHandle object
    AddMessageHandleToHold(I2C_READ_MESSAGE);
    //Create CComponentDispatcherStreamerSetNamePrefixHandle object
    AddMessageHandleToHold(I2C_WRITE_MESSAGE);
    //Create CComponentDispatcherStreamerSetNamePrefixHandle object
    AddMessageHandleToHold(I2C_REALEASE_HANDLE_MESSAGE);
     //Create CComponentDispatcherStreamerSetNamePrefixHandle object
#ifdef DVP_ENABLE
    AddMessageHandleToHold(DVP_ALLOC_KERNEL_NODE);
    AddMessageHandleToHold(DVP_DEINIT);
    AddMessageHandleToHold(DVP_GET_CORE_CAPACITY);
    AddMessageHandleToHold(DVP_IMAGE_ALLOC);
    AddMessageHandleToHold(DVP_IMAGE_CONSTRUCT);
    AddMessageHandleToHold(DVP_IMAGE_FREE);
    AddMessageHandleToHold(DVP_IMAGE_INIT);
    AddMessageHandleToHold(DVP_INIT);
    AddMessageHandleToHold(DVP_INIT_NODE);
    AddMessageHandleToHold(DVP_INIT_PARAMS);
    AddMessageHandleToHold(DVP_KERNEL_GRAPH_DEINIT);
    AddMessageHandleToHold(DVP_KERNEL_GRAPH_INIT);
    AddMessageHandleToHold(DVP_KERNEL_NODE_FREE);
    AddMessageHandleToHold(DVP_PRINT_PERFORMANCE_GRAPH);
    AddMessageHandleToHold(DVP_SET_CORE_CAPACITY);
    AddMessageHandleToHold(DVP_START_K_GRAPH_PROCESS);
#endif

	// All these objects' classes inherit class CComponentDispatcherMessageHandle
	// A constructor initializes the protected dispatcher and ID.
	//Create new Communication channel object with pCommChan(of type CCommunicationChannel*)
    pCommChan = new CCommunicationChannelImplementation(this);

	// Init TCP connection, init COMM_Close_Semaphore, Create Comunication Pipe
	// and Create new thread for internalMsgRecvThread_func
    //COMM_Close_Semaphore is used to protect reading messages from TCP

    if (TIMM_OSAL_ERR_NOT_READY == pCommChan->Init())
    {
        printf("Communication initialization failed!");
        delete(pCommChan);
        return;
    }

	// Create new thread for externalMsgTranThread_func
    pCommChan->Start();

    //joining internalMsgRecvThread
    pCommChan->WaitOnCommObject();
	//Delete Comunication Pipes, Close TCP connection and Destroy Semaphore mutex
    pCommChan->Deinit();
    //printf("Main process: Channel succefully deinited!\n");
    delete(pCommChan);
}

CComponentDispatcherMessageHandle* COMXComponentDispatcher::GetMsgHandler(unsigned int id)
{
    CComponentDispatcherMessageHandle* msgHandle = msgHandlesHold[id];

    if(msgHandle != NULL && msgHandle->Id() == id)
    {
        MMS_IL_PRINT("Selected id: 0x%08X\n", msgHandle->Id());
        return msgHandle;
    }
    else
    {
        MMS_IL_PRINT("Couldn't discover id: 0x%08X\n", id);
        return NULL;
    }
}

TIMM_OSAL_ERRORTYPE COMXComponentDispatcher::MessageEventHandler(void* pMessage)
{
    CComponentDispatcherMessageHandle* msgHandle = GetMsgHandler(((systemMessage_t *)pMessage)->nId);

    if (NULL == msgHandle)
	{
        MMS_IL_PRINT("GetMsgHandler Failed 0x%08X\n", TIMM_OSAL_ERR_MSG_TYPE_NOT_FOUND);
        return TIMM_OSAL_ERR_MSG_TYPE_NOT_FOUND;
    }

    MMS_IL_PRINT("Calling message handle %d\n", msgHandle->Id());

	//Call process function for the given message handle
    ((systemMessage_t*)pMessage)->nStatus = msgHandle->Process(pMessage);
     MMS_IL_PRINT("Message status 0x%08X\n", ((systemMessage_t *)pMessage)->nStatus);

    //Send Acknowledge
	//Send message status over the TCP
    if (TIMM_OSAL_ERR_NONE != pCommChan->Write((systemMessage_t*)pMessage))
    {
        MMS_IL_PRINT("Sendig ACK failed\n");
    }

    return (unsigned int)((systemMessage_t *)pMessage)->nStatus;
}

void COMXComponentDispatcher::GenerateInternalEvent(InternEventMessage eventData)
{
    systemMessage_t outMsg;
    outMsg.nId = SYS_EVENT_NOTIFICATION;
    outMsg.nStatus = MSG_STATUS_OK;
    outMsg.nPayloadSize = sizeof(eventData);
    outMsg.pPayload = &eventData;

    pCommChan->Write((systemMessage_t*)&outMsg);
}

TIMM_OSAL_ERRORTYPE COMXComponentDispatcher::SendMessage(systemMessage_t* outMsg)
{
   return pCommChan->Write(outMsg);
}

void COMXComponentDispatcher::CloseChannel()
{
    pCommChan->Stop();
}

/*--------function prototypes ---------------------------------*/
COMXComponent* COMXComponentDispatcher::GetOMXComponentSlot(unsigned int releaseCompHdl)
{
    COMXComponent* pComp = msgIdComponentTable[releaseCompHdl];
    MMS_IL_PRINT("Component Handle = %d\n", releaseCompHdl);

    if (pComp == NULL)
	{
        MMS_IL_PRINT("No such active handle\n");
        return NULL;
    }

    MMS_IL_PRINT("Component Name = %s\n", (char *) pComp->compName);

    return pComp;
}

COMXComponent* COMXComponentDispatcher::AddOMXComponentSlot(void* copmName)
{
	//Only use the size, as the id's are 0 based. Size would increment after this insertion.
    //Inits the component dispatcher, Inits the OmxComp, inits the callbacks
	COMXComponent* pComp = new COMXComponent(this, (char*)copmName);

    pComp->nComponentId = msgIdComponentTable.size();

    MMS_IL_PRINT("OMX Component 0x%08X with id:  %d\n", (unsigned int)pComp, pComp->nComponentId);
    //Add comp to dictionary
	msgIdComponentTable[pComp->nComponentId] = pComp;

    return pComp;
}

void COMXComponentDispatcher::RemoveOMXComponentSlot(COMXComponent* delComp)
{
    if (msgIdComponentTable[delComp->nComponentId] == NULL)
	{
        MMS_IL_PRINT("No such active handle\n");
		return;
    }

	msgIdComponentTable.erase(delComp->nComponentId);
    delete(delComp);
	delComp = NULL;
}
#ifdef DVP_ENABLE
CDVPComponent* COMXComponentDispatcher::AddDVPComponentSlot()
{
	//Only use the size, as the id's are 0 based. Size would increment after this insertion.
    //Inits the component dispatcher, Inis the OmxComp, inits the callbacks
	CDVPComponent* pComp = new CDVPComponent();

    pComp->nComponentId = DVPComponentTable.size();

	MMS_IL_PRINT("OMX Component 0x%08X with id:  %d\n", pComp, pComp->nComponentId);
    //Add comp to dictionary
	DVPComponentTable[pComp->nComponentId] = pComp;

    return pComp;
}

void COMXComponentDispatcher::RemoveDVPComponentSlot(CDVPComponent* delComp)
{
    if (DVPComponentTable[delComp->nComponentId] == NULL)
	{
        MMS_IL_PRINT("No such active handle\n");
		return;
    }

	DVPComponentTable.erase(delComp->nComponentId);
    delete(delComp);
	delComp = NULL;
}

CDVPComponent* COMXComponentDispatcher::GetDVPComponentSlot(unsigned int ID)
{
    CDVPComponent* pComp = DVPComponentTable[ID];
    MMS_IL_PRINT("DVP Component Handle = %d\n", ID);

    if (pComp == NULL)
	{
        MMS_IL_PRINT("No such active handle\n");
        return NULL;
    }

    MMS_IL_PRINT("Component handle = %d\n", (int)pComp->DVPCompHdl());

    return pComp;
}
#endif