#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortAutoBuffCycleHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherPortAutoBuffCycleHandle::Process(void* pMessage)
{
    COMXComponent* compHdl;
    COMXPortHandle* portData;
    autoBufMngmentMessage_t* autoBufMng;

    bool bEnable = true;

    if(((systemMessage_t *)pMessage)->nStatus != MSG_STATUS_ENABLE)
	{
        bEnable = TIMM_OSAL_FALSE;
    }

    autoBufMng = (autoBufMngmentMessage_t *)((systemMessage_t *)pMessage)->pPayload;

    MMS_IL_PRINT("Setting auto buffer managment to %d on port %d\n", bEnable, autoBufMng->nPortNumber);

    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(autoBufMng->nComponentId);

    if (NULL == compHdl)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	portData = compHdl->GetPortData(autoBufMng->nPortNumber);
	if(portData != NULL)
	{
		portData->bAutoBufferCycle = bEnable;
	}
	else
	{
		return TIMM_OSAL_ERR_OMX;
	}

    return TIMM_OSAL_ERR_NONE;
}