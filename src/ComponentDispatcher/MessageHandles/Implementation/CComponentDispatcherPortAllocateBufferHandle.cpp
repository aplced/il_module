#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortAllocateBufferHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherPortAllocateBufferHandle::Process(void* pMessage)
{
    unsigned int nPortNo = ((allocateMessage_t *)((systemMessage_t *)pMessage)->pPayload)->nPortNumber;
    COMXComponent* pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(((allocateMessage_t *)((systemMessage_t *)pMessage)->pPayload)->nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    //Allocate buffers and requests the component to use them
    return pComp->AllocatePortBuffers(nPortNo);
}