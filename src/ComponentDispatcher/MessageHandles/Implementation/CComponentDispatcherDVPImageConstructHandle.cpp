#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPImageConstructHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPImageConstructHandle::Process(void* pMessage)
{
    DVP_ImageConstructMessage_t* pConstrMsg = (DVP_ImageConstructMessage_t *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(pConstrMsg->nComponentID);

    if (DVP_TRUE != DVPComponentHandle->DVP_Image_Construct((DVP_Image_Params_t*)pConstrMsg->params, pConstrMsg->numParams))
    {
        MMS_IL_PRINT("DVP_Image_Construct failed!\n");
        return TIMM_OSAL_ERR_ALLOC;
    }

    return TIMM_OSAL_ERR_NONE;
}