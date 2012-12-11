#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPImageAllocHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPImageAllocHandle::Process(void* pMessage)
{    
    DVP_ImageAllocMessage_t* pImgAllocMsg = (DVP_ImageAllocMessage_t *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(pImgAllocMsg->nComponentID);

    if (TIMM_OSAL_ERR_NONE != DVPComponentHandle->Image_Alloc(pImgAllocMsg->nImage, pImgAllocMsg->type))
    {
        MMS_IL_PRINT("Image_Alloc failed!\n");
        return TIMM_OSAL_ERR_ALLOC;
    }
    return TIMM_OSAL_ERR_NONE;
}