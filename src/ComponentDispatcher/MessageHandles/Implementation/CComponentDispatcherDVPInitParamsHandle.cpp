#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPInitParamsHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPInitParamsHandle::Process(void* pMessage)
{
    DVP_InitParamsMessage_t* pInitPrm = (DVP_InitParamsMessage_t *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot( pInitPrm->nComponentID);

    DVPComponentHandle->DVP_Init_Params(pInitPrm);

    return TIMM_OSAL_ERR_NONE;
}