#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPDeinitHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPDeinitHandle::Process(void* pMessage)
{
    DVP_U32* nComponentID = (DVP_U32 *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(*nComponentID);

    if(TIMM_OSAL_ERR_NONE != DVPComponentHandle->DVP_Deinit())
    {
        MMS_IL_PRINT("DVP_Deinit failed!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    return TIMM_OSAL_ERR_NONE;
}