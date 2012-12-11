#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherConfigPortServicesHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherConfigPortServicesHandle::Process(void* pMessage)
{
    COMXComponent *compHdl;
    configPortService_t* pConfig;

    MMS_IL_PRINT("Stream service name prefix setting\n");

    pConfig = (configPortService_t *)((systemMessage_t *)pMessage)->pPayload;

    int size = ((systemMessage_t *)pMessage)->nPayloadSize - (sizeof(configPortService_t) - sizeof(TIMM_OSAL_PTR));

    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(pConfig->nComponentId);

    if (NULL == compHdl)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if (!pConfig->pConfig)
	{
        MMS_IL_PRINT("No Configuration is sent!\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    //start new thread waiting on semaphore for set config or set param
    MMS_IL_PRINT("pConfig - %s with size - %d!\n", (char*)pConfig->pConfig, size);
    compHdl->ConfigPortServices(pConfig, size);
    return TIMM_OSAL_ERR_NONE;
}