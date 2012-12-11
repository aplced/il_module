#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPInitHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPInitHandle::Process(void* pMessage)
{    
    DVP_U32* pCompID = (DVP_U32 *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(*pCompID);
    //dispatcher->DVP_Component = DVPComponentHandle;

    if (TIMM_OSAL_ERR_NONE != DVPComponentHandle->DVP_Init())
    {
        MMS_IL_PRINT("DVP_Init failed!\n");
        DVPComponentHandle->DVP_Deinit();
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    return TIMM_OSAL_ERR_NONE;
}