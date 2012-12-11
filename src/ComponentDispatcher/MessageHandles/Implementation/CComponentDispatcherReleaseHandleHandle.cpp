#include "inc/ComponentDispatcher/MessageHandles/Implementation/CComponentDispatcherReleaseHandleHandle.h"

TIMM_OSAL_ERRORTYPE CComponentDispatcherReleaseHandleHandle::Process(void* pMessage)
{
    COMXComponent* pComp;
    releaseHandleMessage_t* relHdlMSg = ((releaseHandleMessage_t *)((systemMessage_t *)pMessage)->pPayload);

    MMS_IL_PRINT("Component Handle = %d\n", relHdlMSg->nComponentId);

    pComp = dispatcher->GetOMXComponentSlot(relHdlMSg->nComponentId);
    if (NULL == pComp)
	{
        MMS_IL_PRINT("Unknown component\n");
        return TIMM_OSAL_ERR_UNKNOWN;
    }
    //Free component and remove from dictionary
    dispatcher->RemoveOMXComponentSlot(pComp);

    return TIMM_OSAL_ERR_NONE;
}
