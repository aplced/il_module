#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPStartKGraphProcessHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPStartKGraphProcessHandle::Process(void* pMessage)
{
    
    DVP_U32* nComponentID = (DVP_U32 *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(*nComponentID);
    if (DVPComponentHandle->DVPCompHdl())
    {
        DVPComponentHandle->DVP_Start();
    } else
    {
        MMS_IL_PRINT("DVP not initizlized!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    return TIMM_OSAL_ERR_NONE;
}