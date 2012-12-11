#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPAllocKernelNodeHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPAllocKernelNodeHandle::Process(void* pMessage)
{
    DVP_KAlloc_Message_t* kAlloc = (DVP_KAlloc_Message_t*)((systemMessage_t *)pMessage)->pPayload; 

    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(kAlloc->nComponentID);

    if (TIMM_OSAL_ERR_NONE != DVPComponentHandle->KernelNode_Alloc())
        return TIMM_OSAL_ERR_ALLOC;

    return TIMM_OSAL_ERR_NONE;
}