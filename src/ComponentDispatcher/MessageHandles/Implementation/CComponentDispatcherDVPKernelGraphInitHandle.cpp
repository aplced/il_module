#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPKernelGraphInitHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPKernelGraphInitHandle::Process(void* pMessage)
{
    DVP_U32* pCompID = (DVP_U32 *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->AddDVPComponentSlot();

    if (TIMM_OSAL_ERR_NONE != DVPComponentHandle->KernelGraph_Init())
    {
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    * pCompID = DVPComponentHandle->nComponentID;
    return TIMM_OSAL_ERR_NONE;
}