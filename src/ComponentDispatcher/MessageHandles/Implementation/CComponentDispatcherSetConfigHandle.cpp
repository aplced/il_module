#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherSetConfigHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherSetConfigHandle::Process(void* pMessage)
{
	int eError;
    unsigned int nOffset = 0;
    unsigned int nPayloadSize = ((systemMessage_t *)pMessage)->nPayloadSize;
    configMessage_t* configMessage = (configMessage_t*)((systemMessage_t *)pMessage)->pPayload;
    COMXComponent* pComp;

    while(nOffset < nPayloadSize)
	{
        pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(((configMessage_t *)((systemMessage_t *)pMessage)->pPayload)->nComponentId);

        if (NULL == pComp)
		{
            MMS_IL_PRINT("Unknown component\n");
            return TIMM_OSAL_ERR_UNKNOWN;
        }

        eError = pComp->SetConfig((OMX_INDEXTYPE)configMessage->nStructureId, configMessage->pConfigData);
        if (eError != 0)
		{
            MMS_IL_PRINT("OMX_SetConfig failed with error 0x%08X\n", eError);
            return TIMM_OSAL_ERR_OMX;
        }

        pComp->ConfigAllPortServices();
        // Work arround for incorrect system structure size from script standalone message generation
        // - multiple structure configurations are not used anyway...
        if(configMessage->nStructureSize == 0)
            nOffset = nPayloadSize;
        nOffset += configMessage->nStructureSize;
        configMessage = (configMessage_t *)((TIMM_OSAL_U8 *)configMessage + configMessage->nStructureSize);
    }

    return TIMM_OSAL_ERR_NONE;
}