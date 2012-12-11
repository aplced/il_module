#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPKernelNodeFreeHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPKernelNodeFreeHandle::Process(void* pMessage)
{   
    DVP_U32* nComponentID = (DVP_U32 *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(*nComponentID);

    if (TIMM_OSAL_ERR_NONE != DVPComponentHandle->KernelNode_Free())
    {
        MMS_IL_PRINT("KernelNode_Free failed!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    return TIMM_OSAL_ERR_NONE;
}