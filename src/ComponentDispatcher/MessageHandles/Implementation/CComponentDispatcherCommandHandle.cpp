#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherCommandHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherCommandHandle::Process(void* pMessage)
{
   COMXComponent* pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(((configMessage_t *)((systemMessage_t *)pMessage)->pPayload)->nComponentId);

    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    // Process Command Message
    // Calls OMX_SendCommand for all messages
    // handling command specific situations
       pComp->Command(((systemMessage_t *)pMessage)->pPayload, pComp);

    return TIMM_OSAL_ERR_NONE;
}