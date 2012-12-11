#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherDSSConfigHandle.h"
#include "inc/Display/CDisplay.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherDSSConfigHandle::Process(void* pMessage)
{
    displayConfig_t* pConfig = (displayConfig_t*)((systemMessage_t *) pMessage)->pPayload;
    COMXComponent* compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(pConfig->nComponentId);
    messageStatuses_t status = (messageStatuses_t)((systemMessage_t *) pMessage)->nStatus;

    if (status == MSG_STATUS_SET)
	{
        MMS_IL_PRINT("SET message received\n");
		compHdl->displayModule->Config(&pConfig->Config);
    } else if (status == MSG_STATUS_GET)
	{
        MMS_IL_PRINT("GET message received\n");
		compHdl->displayModule->GetConfig(&pConfig->Config);
    }
	else
	{
        MMS_IL_PRINT("Unrecognized message status (%d)\n", status);
    }

    return TIMM_OSAL_ERR_NONE;
}