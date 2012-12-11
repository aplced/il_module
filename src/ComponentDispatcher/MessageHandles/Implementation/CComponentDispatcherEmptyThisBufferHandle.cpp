#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherEmptyThisBufferHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherEmptyThisBufferHandle::Process(void* pMessage)
{
    COMXComponent *compHdl;

    allocateMessage_t* portCdm = (allocateMessage_t*)((systemMessage_t *)pMessage)->pPayload;
    //For the given component free all buffers allocated for
    //it's port
    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(portCdm->nComponentId);

    if (NULL == compHdl)
	{
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if (((systemMessage_t *)pMessage)->nStatus == MSG_STATUS_OK)
    {
        //Gets port definitions and handle and frees all allocated buffers
        if(compHdl->QueueEmptyPortBuffers(portCdm->nPortNumber, portCdm->nBuffNumber, false) != OMX_ErrorNone)
            return TIMM_OSAL_ERR_OMX;

    } else if (((systemMessage_t *)pMessage)->nStatus == MSG_STATUS_DATA_MORE)
    {
        if(compHdl->QueueEmptyPortBuffers(portCdm->nPortNumber, portCdm->nBuffNumber, true) != OMX_ErrorNone)
	        return TIMM_OSAL_ERR_OMX;
    }

    return TIMM_OSAL_ERR_NONE;

}