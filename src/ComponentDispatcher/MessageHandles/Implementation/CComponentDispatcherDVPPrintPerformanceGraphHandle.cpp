#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDVPPrintPerformanceGraphHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDVPPrintPerformanceGraphHandle::Process(void* pMessage)
{
    DVP_U32* nComponentID = (DVP_U32 *)((systemMessage_t *)pMessage)->pPayload;
    CDVPComponent *DVPComponentHandle = dispatcher->GetDVPComponentSlot(*nComponentID);

    DVPComponentHandle->PrintPerformanceGraph();

    return TIMM_OSAL_ERR_NONE;
}