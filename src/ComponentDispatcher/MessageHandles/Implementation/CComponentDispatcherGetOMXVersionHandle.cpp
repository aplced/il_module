#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherGetOMXVersionHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherGetOMXVersionHandle::Process(void* pMessage)
{
    COMXComponent *compHdl;

    getOMXVersion_t* getOMXVer = (getOMXVersion_t*)((systemMessage_t *)pMessage)->pPayload;

    compHdl = (COMXComponent*)dispatcher->GetOMXComponentSlot(getOMXVer->nComponentId);

    if (NULL == compHdl)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }

	return compHdl->GetComponentVersion((OMX_VERSIONTYPE*) &(getOMXVer->nCompVersion), (OMX_VERSIONTYPE*) &(getOMXVer->nSpecVersion), (OMX_UUIDTYPE*) &(getOMXVer->cCompUUID[0]));
}