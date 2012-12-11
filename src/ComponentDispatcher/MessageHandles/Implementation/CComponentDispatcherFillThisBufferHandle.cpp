#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherFillThisBufferHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherFillThisBufferHandle::Process(void* pMessage)
{
    COMXComponent *compHdl;

    allocateMessage_t* portCdm = (allocateMessage_t*)((systemMessage_t *)pMessage)->pPayload;

    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(portCdm->nComponentId);

    if (NULL == compHdl)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if(compHdl->QueueFillPortBuffers(portCdm->nPortNumber, portCdm->nBuffNumber) != OMX_ErrorNone)
	{
        return TIMM_OSAL_ERR_OMX;
    }

    return TIMM_OSAL_ERR_NONE;

}