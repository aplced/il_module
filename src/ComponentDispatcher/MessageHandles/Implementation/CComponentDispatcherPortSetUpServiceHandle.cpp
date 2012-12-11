#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherPortSetUpServiceHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherPortSetUpServiceHandle::Process(void* pMessage)
{

    COMXComponent* pComp;
   // int nPayloadSize = ((systemMessage_t *)pMessage)->nPayloadSize;
    setUpPortSrvcMessage_t* suPortSrvc =((setUpPortSrvcMessage_t *)((systemMessage_t *)pMessage)->pPayload);
    int nServiceHeadSize = ((systemMessage_t *)pMessage)->nPayloadSize - sizeof(setUpPortSrvcMessage_t);
    pComp = (COMXComponent*)dispatcher->GetOMXComponentSlot(suPortSrvc->nComponentId);
    //MMS_IL_PRINT("Payload Size = %d\n", nPayloadSize);
    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

    if(((systemMessage_t *)pMessage)->nStatus == MSG_STATUS_ENABLE)
	{
        return pComp->AddPortSevice(suPortSrvc->nPortNumber, suPortSrvc->nServiceType, suPortSrvc->pServiceHeader, nServiceHeadSize);
    }
	else
	{
        return pComp->RemovePortService(suPortSrvc->nPortNumber, suPortSrvc->nServiceType);
    }
}