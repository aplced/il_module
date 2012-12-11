#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPInitNodeHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPInitNodeHandle::Process(void* pMessage)
{
    DVP_InitNodeMessage_t* pInitNodeMsg = (DVP_InitNodeMessage_t *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(pInitNodeMsg->nComponentID);

    DVPComponentHandle->Node_Init(pInitNodeMsg->nNode, pInitNodeMsg->nInput, pInitNodeMsg->nOutput,
                                                        pInitNodeMsg->kernel, pInitNodeMsg->affinity);

    return TIMM_OSAL_ERR_NONE;
}