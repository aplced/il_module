#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetHandleHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherGetHandleHandle::Process(void* pMessage)
{
    //Allocate and add given component to  msgIdComponentTable map
    //Init callbacks
    COMXComponent* pComp = dispatcher->AddOMXComponentSlot(((getHandleMessage_t *)((systemMessage_t *)pMessage)->pPayload)->cComponentName);

    if (OMX_ErrorNone != pComp->GetOMXCompHdl())
    {
        MMS_IL_PRINT("Cannot create OMX handle\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    getHandleMessageAck_t* getHdlAck = (getHandleMessageAck_t*)malloc(((systemMessage_t *)pMessage)->nPayloadSize + sizeof(unsigned int));
    if(getHdlAck == NULL)
	{
        MMS_IL_PRINT("Insufficient resources - can't allocate memory!\n");
        return TIMM_OSAL_ERR_ALLOC;
    }
                  //destination                                     source
    memcpy((TIMM_OSAL_U8*)getHdlAck + sizeof(unsigned int), ((systemMessage_t *)pMessage)->pPayload, ((systemMessage_t *)pMessage)->nPayloadSize);
    getHdlAck->nCompId = pComp->nComponentId;
    free(((systemMessage_t *)pMessage)->pPayload);
    ((systemMessage_t *)pMessage)->pPayload = getHdlAck;
    ((systemMessage_t *)pMessage)->nPayloadSize += sizeof(unsigned int);
    //Prepare ack
    return TIMM_OSAL_ERR_NONE;
}

