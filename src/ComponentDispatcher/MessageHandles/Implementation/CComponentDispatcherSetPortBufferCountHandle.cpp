#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetPortBufferCountHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherSetPortBufferCountHandle::Process(void* pMessage)
{
    COMXComponent *compHdl;
    COMXPortHandle* portData;
    allocateExtraMessage_t* extraBfrCnt;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;

    extraBfrCnt = (allocateExtraMessage_t *)((systemMessage_t *)pMessage)->pPayload;

    MMS_IL_PRINT("Setting %d actual buffers on port %d\n", extraBfrCnt->nExtraBufferCount, extraBfrCnt->nPortNumber);

    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(extraBfrCnt->nComponentId);

    if (NULL == compHdl)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	portData = compHdl->GetPortData(extraBfrCnt->nPortNumber);
    if(portData == NULL)
	{
		MMS_IL_PRINT("Failed to get port %d data\n",  extraBfrCnt->nPortNumber);
		return TIMM_OSAL_ERR_OMX;
	}

    portData->tPortDef.nBufferCountActual = extraBfrCnt->nExtraBufferCount;
    //Send to component
    eError = compHdl->SetParam(OMX_IndexParamPortDefinition, (unsigned char*)&(portData->tPortDef));
    if (eError != OMX_ErrorNone)
	{
        MMS_IL_PRINT("OMX set port params failed\n");
        return TIMM_OSAL_ERR_OMX;
    }
    MMS_IL_PRINT("Port %lu actual buffers count: %lu\n", portData->tPortDef.nPortIndex, portData->tPortDef.nBufferCountActual);

    return TIMM_OSAL_ERR_NONE;
}