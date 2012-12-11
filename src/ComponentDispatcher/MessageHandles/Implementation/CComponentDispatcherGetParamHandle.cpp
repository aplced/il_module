#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetParamHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherGetParamHandle::Process(void* pMessage)
{
    int eError = 0;
    unsigned int nOffset = 0;
    unsigned int nPayloadSize = ((systemMessage_t *)pMessage)->nPayloadSize;
    configMessage_t* pConfig = (configMessage_t*)((systemMessage_t *)pMessage)->pPayload;
    COMXComponent* pComp;

    for (;nOffset < nPayloadSize;) 
	{
        pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(((configMessage_t *)((systemMessage_t *)pMessage)->pPayload)->nComponentId);

        if (NULL == pComp)
		{
            MMS_IL_PRINT("Unknown component\n");
            return TIMM_OSAL_ERR_UNKNOWN;
        }

        // Process Get Param Message
		eError = pComp->GetParam((OMX_INDEXTYPE)pConfig->nStructureId, pConfig->pConfigData);

        // Work arround for incorrect system structure size from script standalone message generation
        // - multiple structure configurations are not used anyway...
        if(pConfig->nStructureSize == 0)
            nOffset = nPayloadSize;
        // Update Offset 
        nOffset += pConfig->nStructureSize;
        // Find the Beggining of next config structure 
        pConfig = (configMessage_t *)((TIMM_OSAL_U8 *)pConfig + pConfig->nStructureSize);
    }

    MMS_IL_PRINT("OMX_GetParameter - 0x%08x\n", eError);
    return eError;
}