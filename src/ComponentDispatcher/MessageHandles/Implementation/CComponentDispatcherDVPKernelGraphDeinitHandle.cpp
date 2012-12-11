#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPKernelGraphDeinitHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPKernelGraphDeinitHandle::Process(void* pMessage)
{    
    
    DVP_U32* pCompID = (DVP_U32 *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(*pCompID);

    if (TIMM_OSAL_ERR_NONE != DVPComponentHandle->KernelGraph_Deinit())
    {
        MMS_IL_PRINT("KernelGraph_Deinit failed!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    dispatcher->RemoveDVPComponentSlot(DVPComponentHandle);

    return TIMM_OSAL_ERR_NONE;
}