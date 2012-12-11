#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPImageInitHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPImageInitHandle::Process(void* pMessage)
{    
    
    DVP_ImageInitMessage_t* pImgInit = (DVP_ImageInitMessage_t *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(pImgInit->nComponentID);

    DVPComponentHandle->Image_Init(pImgInit->nImage, pImgInit->color);

    return TIMM_OSAL_ERR_NONE;
}